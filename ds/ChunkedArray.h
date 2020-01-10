#ifndef __CHUNKED_ARRAY_H__
#define __CHUNKED_ARRAY_H__


#define CHUNK_SIZE 256
#define MAX_CHUNK  16

#include <bitset>
#include <stdint.h>
#include <cstdlib>

inline size_t getblockSize( uint8_t blockpos, uint8_t remainder) {
  const size_t blocksize [] = {0, 256, 512, 768, 1024, 1280, 1536, 1792, 2048, 2304, 2560, 2816, 3072, 3328, 3584, 3840,4096};
  return blocksize[blockpos] + remainder;
}

struct ChunkHeader{
/*
   ChunkHeader() : // error: use of deleted function ‘Block::Block()’
    _empty(1)
   ,_start(0)
   ,_continuesBlocks(0)
   ,_lastBlockSize(0)
   {
   }
*/
   uint8_t _empty : 1; //is the block is empty ?? // can i move this to emptyset list
   uint8_t _start : 1; //is this start of a block //can I move to start bitset
   uint8_t _continuesBlocks : 4; /* stores continues block size, default 0 , max 15. i.e 256/16 = 16, so 4 bytes to represent chunks
       any data > CHUNK_SIZE * MAX_CHUNK = 4K will be discarded */
   uint8_t _lastBlockSize;  /*number of chars allocated for last block */

   size_t size() const {
#if 0
      return ( CHUNK_SIZE * _continuesBlocks )  + _lastBlockSize ;
#else
   if(!_continuesBlocks) return _lastBlockSize +1;
   return (256<< (_continuesBlocks-1)) + _lastBlockSize ;

#endif
   }
   void reset() {
     _empty = 1;
     _start = 0;
     _continuesBlocks = 0;
    _lastBlockSize = 0;
   }
};

struct Block {
   union{
      struct {
        struct ChunkHeader _header;
        uint8_t _data[CHUNK_SIZE - sizeof(struct ChunkHeader)];
     } _chunk;
     uint8_t _data[CHUNK_SIZE];
   } _data;
   void reset() { _data._chunk._header.reset();}
};

template<size_t N = MAX_CHUNK>
struct ArrayBlock
{
  Block _blocks[N];

  uint8_t* start()const {return 0; }
  //use bit_util.h from arrow for bit manuplation 
  uint32_t _emptySet = 0;
  uint32_t _startSet = 0;
  void reset() {
  }
};

struct ArraYBloack
{

};
#define TEST 1
#ifdef TEST
   int main(void) {
     ChunkHeader header;
     header.reset();
     Block block;
     block.reset();
     ArrayBlock<> array;
     array.reset();
     return 0;
   }

#endif

#endif //__CHUNKED_ARRAY_H__
