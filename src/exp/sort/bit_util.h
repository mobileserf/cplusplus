#ifndef __MSERF_BIT_UTIL_H__
#define __MSERF_BIT_UTIL_H__

//from arrow/util/bit_util.h
#include <endian.h>
#ifndef __BYTE_ORDER__
#error "__BYTE_ORDER__ not defined"
#endif

#ifndef __ORDER_LITTLE_ENDIAN__
#error "__ORDER_LITTLE_ENDIAN__ not defined"
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ARROW_LITTLE_ENDIAN 1
#else
#define ARROW_LITTLE_ENDIAN 0
#endif

#define ARROW_BYTE_SWAP64 __builtin_bswap64
#define ARROW_BYTE_SWAP32 __builtin_bswap32

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <immintrin.h>

#include "macros.h"

//how many bytes are required to store a number 0-0, [1-8]-1, [9-16]-2, [17-24]-3 [25-30]-4
constexpr int64_t BytesForBits(int64_t bits) { return (bits + 7) >> 3; }

constexpr bool IsPowerOf2(int64_t value) {
  return value > 0 && (value & (value - 1)) == 0;
}

constexpr bool IsPowerOf2(uint64_t value) {
  return value > 0 && (value & (value - 1)) == 0;
}


// Returns the smallest power of two that contains v.  If v is already a
// power of two, it is returned as is.
static inline int64_t NextPower2(int64_t n) {
  // Taken from
  // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n |= n >> 32;
  n++;
  return n;
}

constexpr bool IsMultipleOf64(int64_t n) { return (n & 63) == 0; }

constexpr bool IsMultipleOf8(int64_t n) { return (n & 7) == 0; }


// \brief Count the number of leading zeros in an unsigned integer.
static inline int CountLeadingZeros(uint32_t value) {
  if (value == 0) return 32;
  return static_cast<int>(__builtin_clz(value));
}

static inline int CountLeadingZeros(uint64_t value) {
  if (value == 0) return 64;
  return static_cast<int>(__builtin_clzll(value));
}

static inline int CountTrailingZeros(uint32_t value) {
  if (value == 0) return 32;
  return static_cast<int>(__builtin_ctzl(value));
}

static inline int CountTrailingZeros(uint64_t value) {
  if (value == 0) return 64;
  return static_cast<int>(__builtin_ctzll(value));
}
#endif  // __MSERF_BIT_UTIL_H__
