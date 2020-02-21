//g++ avl_array_sort.cpp -O2  -mavx -msse4 -std=c++11
//perf record --call-graph dwarf ./a.out 10240
// perf report --call-graph
//time perf stat -d -d -d ./a.out 10240
#include <cstdint>
#include  <cstring>
#include <iostream>
#include <array>
#include <vector>
#include "avxmem.h"
#define MAX_OPERATION 10240
#define MAX_ENTRY_IN_AVL 2048
//#define MAX_ENTRY_IN_AVL 512

#define SIMD 1

template<typename size_type, const size_type Size, const bool Fast = true>
class avl_array
{
  typedef size_type type;
#define SMALL  1
#ifdef SMALL
  typedef size_type position_type;
  typedef size_type key_type;
  //typedef uint64_t key_type;
#else
  typedef uint64_t position_type;
  typedef uint64_t key_type;
#endif
  //typename Size Length;

  struct KeyIndex {
    bool operator()(const KeyIndex other) {
      return (this->key < other.key);
    }
    bool operator<(const KeyIndex other)  {
      return (this->key < other.key);
    }
    key_type key;
    position_type index;
  };

  uint64_t _size;
public:
  alignas(128) std::array<KeyIndex, Size> _keyIndex;               // node childs
  alignas(128)std::array<size_type, Size> _freeIndex;               // free position
  using KeyIter = typename std::array<KeyIndex, Size>::iterator;
  using KeyIterC = typename std::array<KeyIndex, Size>::const_iterator;

  avl_array():_size(0){
    memset(&_keyIndex, 0, sizeof(_keyIndex));
    position_type max = Size;
    position_type j = 0;
    while(max) { _freeIndex[j++] = --max; } //insert free index ivalue in reverse order 10,9,8,7...0
  }
  KeyIter find(const key_type key) {
     KeyIter end = _keyIndex.begin();
     std::advance(end, _size);
     KeyIndex mykey;
     mykey.key=key;
     KeyIter iter = std::lower_bound(_keyIndex.begin(), end, mykey);
     //   [](const KeyIndex a , const KeyIndex b) -> bool { return a.key < b.key; });
     return iter;
  }
  //inserts and return position
  uint64_t insert(const key_type key) __attribute__((noinline)) {
    if(_size == Size) return -1;
    auto iter = find(key);
    if(iter->key != key ) {
       auto distance = std::distance(_keyIndex.begin(), iter);
       ++_size;
#ifdef SIMD
       AVX_memmove(&_keyIndex[distance+1], &_keyIndex[distance], sizeof(KeyIndex)*(_size-distance));
#else
       memmove(&_keyIndex[distance+1], &_keyIndex[distance], sizeof(KeyIndex)*(_size-distance));
#endif
       _keyIndex[distance].key = key;
       _keyIndex[distance].index = _freeIndex[Size-_size];
       return _keyIndex[distance].index;
    }
    return iter->index;

  }
  uint64_t erase(const key_type key) __attribute__((noinline)) {
    auto iter = find(key);
    if(iter->key == key ) {
       auto distance = std::distance(_keyIndex.begin(), iter);
       position_type index = iter->index;
#ifdef SIMD
       AVX_memcpy(&_keyIndex[distance], &_keyIndex[distance+1], sizeof(KeyIndex)*(_size-distance));
#else
       memcpy(&_keyIndex[distance], &_keyIndex[distance+1], sizeof(KeyIndex)*(_size-distance));
#endif
       --_size;
       _freeIndex[Size-_size] = index;
       return index;
    }
    return -1;
  }
  void  print() __attribute__((noinline)) {
     KeyIter begin = _keyIndex.begin();
     KeyIter end = _keyIndex.begin();
     std::advance(end, _size);
     while( begin !=end) { std::cout<<begin->key<<" : "<<begin->index<<" "; ++begin;}
     std::cout<<std::endl;
  }

};

#include <bits/stdc++.h>
#include <string>
#include <unordered_map>

using namespace std;

const int MAX = 26;

// Returns a string of random alphabets of
// length n.
string printRandomString(int n)
{
    char alphabet[MAX] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                          'h', 'i', 'j', 'k', 'l', 'm', 'n',
                          'o', 'p', 'q', 'r', 's', 't', 'u',
                          'v', 'w', 'x', 'y', 'z' };

    string res = "";
    for (int i = 0; i < n; i++)
        res = res + alphabet[rand() % MAX];

    return res;
}
typedef avl_array<std::uint32_t, MAX_ENTRY_IN_AVL, true> AVLTREE;

void insert(AVLTREE& avl, int maxCount) {
    for(int i = 0 ; i < maxCount; ++i) {
        int key = rand() % MAX_ENTRY_IN_AVL;
        auto ret = avl.insert(key);
        if(-1 == ret) {
           std::cout<<key<< " :Failed to insert "<<key<<std::endl;
        }
        else{
           std::cout<<key<< " : " <<ret<<std::endl;
        }
    }

}
void erase(AVLTREE& avl, int maxCount) {
    int missedErase = 0;
    for(int i = 0 ; i < maxCount; ++i) {
        int key = rand() % MAX_ENTRY_IN_AVL;
        auto ret = avl.erase(key);
        if(-1 == ret) {
          ++missedErase;
        }
        else {
           std::cout<<key<< " : " <<ret<<" ERASED"<<std::endl;
        }
    }
   std::cout<<"Missed Erase: "<<missedErase<<std::endl;
}

#define LOAD_TEST 1
int main(int argc, char**argv ) {
#ifdef LOAD_TEST
    int maxkey = atoi(argv[1]);
    std::vector< std::string> keys(maxkey);
    std::vector<AVLTREE* > hash_map(maxkey);
    for(int i = 0 ; i < maxkey; ++i) {
       std::string str = printRandomString(10);
       keys.push_back(str);
       hash_map[i] = new AVLTREE();
    }
    size_t i = 0;

    const int prefecthcount = 10;
    std::vector<int> _prefetch_key(prefecthcount);
    for( ; i < maxkey*MAX_OPERATION; i+=prefecthcount) {

       for(int j = 0; j < prefecthcount; ++j) {
          int keypos = rand() % maxkey;
          __builtin_prefetch(static_cast<const void*>(hash_map[keypos]));
          _prefetch_key[j] = keypos;
       }

       for(int j = 0; j < prefecthcount; ++j) {
         auto value = hash_map[_prefetch_key[j]];
         int key = rand() % MAX_ENTRY_IN_AVL;
         if(rand() % 2)
           value->erase(key);
         else
           value->insert(key);
      }

    }

   std::cout<<i<<std::endl;
#else
    AVLTREE avl;
    int maxCount =  atoi(argv[1]);
    insert(avl, maxCount);
    avl.print();
    erase(avl, maxCount);
    avl.print();
    insert(avl, maxCount);
    avl.print();
#endif
    return 0;
}
