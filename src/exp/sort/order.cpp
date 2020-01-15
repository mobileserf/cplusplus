// g++ order.cpp  -O3 --std=c++11 -mavx2

#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <stdint.h>
#include <algorithm>
#include <random>
#include <climits>
#include <type_traits>

#define NDEBUG

#include "macros.h"
#include "bit_util.h"
//little-endian _MM_SHUFFLE
//#define SHUF(i0, i1, i2, i3) (i0 + i1*4 + i2*16 + i3*64)
#define SHUF(i0, i1, i2, i3) _MM_SHUFFLE(i0, i1, i2, i3)

#define TESTINLINE NOINLINE


#if 1
FORCEINLINE uint32_t bsr(uint32_t x) { //bsr
        return 31 - __builtin_clz(x);
}

FORCEINLINE uint32_t bsf(uint32_t x) { //
    return __builtin_ctz(x);
}
#else
#define bsr CountLeadingZeros
#define bsf CountLeadingZeros

#endif

//array size must be power of 2
//search for array size <=64 )

/*
           32 byte: linear_search_sse_UR(4.5ns), linear_search_avx_UR, hybridX_search(5.4)
           64 byte: linear_search_avx_UR(6.0 - 6.6),  hybridX_search, linear_search_sse_UR(6.6)
           128 byte: hybridX_search(8.3), linear_search_avx_UR, binary_search_branchless(9.5)
           256 byte: hybridX_search(9.5), binary_search_branchless(11.5), linear_search_avx_UR(14.3)
           512 byte:  hybridX_search, binary_search_branchless, quaternary_search_branchless(14.3)
           1024 byte:  hybridX_search, binary_search_branchless(9.5 ns)
*/


intptr_t MINUS_ONE = -1;
static TESTINLINE int hybridX_search (const int *arr, int n, int key) {
    assert((n & (n+1)) == 0); //n = 2^k - 1
    intptr_t pos = MINUS_ONE;
    intptr_t logstep = bsr(n);
    intptr_t step = intptr_t(1) << logstep;
    while (step > 8) {
        pos = (arr[pos + step] < key ? pos + step : pos);
        step >>= 1;
    }
    pos++;
    step <<= 1;

    assert(size_t(arr) % 16 == 0);
    assert(pos % 16 == 0);
    __m128i vkey = _mm_set1_epi32(key);
    __m128i cnt = _mm_setzero_si128();
    __m128i cmp0 = _mm_cmpgt_epi32 (vkey, _mm_load_si128((__m128i *)&arr[pos+0]));
    __m128i cmp1 = _mm_cmpgt_epi32 (vkey, _mm_load_si128((__m128i *)&arr[pos+4]));
    __m128i cmp2 = _mm_cmpgt_epi32 (vkey, _mm_load_si128((__m128i *)&arr[pos+8]));
    __m128i cmp3 = _mm_cmpgt_epi32 (vkey, _mm_load_si128((__m128i *)&arr[pos+12]));
    __m128i pack01 = _mm_packs_epi32 (cmp0, cmp1);
    __m128i pack23 = _mm_packs_epi32 (cmp2, cmp3);
    __m128i pack0123 = _mm_packs_epi16 (pack01, pack23);
    uint32_t res = _mm_movemask_epi8 (pack0123);

    return pos + bsf(~res);
}

template<intptr_t MAXN> static TESTINLINE int linear_search_avx_UR (const int *arr, int n, int key) {
    assert(size_t(arr) % 32 == 0);
    assert(n <= 1024);
    __m256i vkey = _mm256_set1_epi32(key);
    __m256i cnt = _mm256_setzero_si256();
    intptr_t i = 0;
    #define STEP \
    if (i < MAXN) {\
        __m256i mask0 = _mm256_cmpgt_epi32(vkey, _mm256_load_si256((__m256i *)&arr[i+0])); \
        __m256i mask1 = _mm256_cmpgt_epi32(vkey, _mm256_load_si256((__m256i *)&arr[i+8])); \
        __m256i sum = _mm256_add_epi32(mask0, mask1); \
        cnt = _mm256_sub_epi32(cnt, sum); \
    } i += 16;
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    #undef STEP
    __m128i xcnt = _mm_add_epi32(_mm256_extracti128_si256(cnt, 1), _mm256_castsi256_si128(cnt));
    xcnt = _mm_add_epi32(xcnt, _mm_shuffle_epi32(xcnt, SHUF(2, 3, 0, 1)));
    xcnt = _mm_add_epi32(xcnt, _mm_shuffle_epi32(xcnt, SHUF(1, 0, 3, 2)));
    return _mm_cvtsi128_si32(xcnt);
}

static TESTINLINE int binary_search_branchless (const int *arr, int n, int key) {
    assert((n & (n+1)) == 0); //n = 2^k - 1
    //intptr_t pos = -1;            //generates "or r9, -1" on MSVC -- false dependency harms throughput
    intptr_t pos = MINUS_ONE;       //workaround for MSVC: generates mov without dependency
    intptr_t logstep = bsr(n);
    intptr_t step = intptr_t(1) << logstep;
    while (step > 0) {
        pos = (arr[pos + step] < key ? pos + step : pos);
        step >>= 1;
    }
    return pos + 1;
}

template<intptr_t MAXN> static TESTINLINE int linear_search_sse_UR (const int *arr, int n, int key) {
    assert(size_t(arr) % 16 == 0);
    assert(n <= 1024);
    __m128i vkey = _mm_set1_epi32(key);
    __m128i cnt = _mm_setzero_si128();
    intptr_t i = 0;
    #define STEP \
    if (i < MAXN) {\
        __m128i mask0 = _mm_cmplt_epi32(_mm_load_si128((__m128i *)&arr[i+0]), vkey); \
        __m128i mask1 = _mm_cmplt_epi32(_mm_load_si128((__m128i *)&arr[i+4]), vkey); \
        __m128i mask2 = _mm_cmplt_epi32(_mm_load_si128((__m128i *)&arr[i+8]), vkey); \
        __m128i mask3 = _mm_cmplt_epi32(_mm_load_si128((__m128i *)&arr[i+12]), vkey); \
        __m128i sum = _mm_add_epi32(_mm_add_epi32(mask0, mask1), _mm_add_epi32(mask2, mask3)); \
        cnt = _mm_sub_epi32(cnt, sum); \
    } i += 16;
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    STEP STEP STEP STEP STEP STEP STEP STEP
    #undef STEP
    cnt = _mm_add_epi32(cnt, _mm_shuffle_epi32(cnt, SHUF(2, 3, 0, 1)));
    cnt = _mm_add_epi32(cnt, _mm_shuffle_epi32(cnt, SHUF(1, 0, 3, 2)));
    return _mm_cvtsi128_si32(cnt);
}

template<intptr_t MAX_LEN> static TESTINLINE int search_sorted_array(const int *arr, int len, int key)
{
  static_assert(IsPowerOf2(MAX_LEN), "search_sorted_array - should use a power of 2 as template parameter");
  static_assert((MAX_LEN >=32) && (MAX_LEN<=1024), "search_sorted_array - minimum value should be 32 and maximum is 1024");
  int result = -1;
  //TODO, use enable_if<>
  switch(MAX_LEN)
  {
     case 32:
         result = linear_search_sse_UR<MAX_LEN>(arr, len, key);
         break;
     case 64:
         result = linear_search_avx_UR<MAX_LEN>(arr, len, key);
         break;
     case 128:
         result = hybridX_search(arr, len, key);
         break;
     default:
         result = binary_search_branchless(arr, len, key); //best one
         break;
   }
   //return arr[result] != key ? -1:result;
   return result;
}

int main() {

  ALIGN(32) int arr[256] = {11,22,33};
  int key = 33;
  int len = 3;
  int pos = search_sorted_array<256>(arr, len, key);
  std::cout<<pos<<std::endl;
  pos = search_sorted_array<128>(arr, len, key);
  std::cout<<pos<<std::endl;
  /* TODO, following 2 are failing */
  pos = search_sorted_array<64>(arr, len, key);
  std::cout<<pos<<std::endl;
  pos = search_sorted_array<32>(arr, len, key);
  std::cout<<pos<<std::endl;
  return 0;
}
