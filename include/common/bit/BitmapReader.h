#ifndef __MSERF_BIT_UTIL_BITMAP_READER_H__
#define __MSERF_BIT_UTIL_BITMAP_READER_H__

//CREDIT: taken from arrow-master/cpp/src/arrow/util
//
#include <cstdint>
#include <cstring>
#include <iostream>
#include <ostream>
#include <bitset>

#include "Mserf/util/BitUtil.h"

namespace Mserf {

namespace util {


class BitmapReader {
 public:
  /*
   * start_offset - start position of bitmap, this value must be >=0 & < length
   * length - total number of bit to look */
  BitmapReader(const uint8_t* bitmap, int64_t start_offset, int64_t length)
      : bitmap_(bitmap), position_(start_offset), length_(length)
      ,current_byte_(0), byte_offset_(0), bit_offset_(8) {
    if (length > 0 && position_<length) {
      byte_offset_ = start_offset / 8;
      bit_offset_ = start_offset % 8;
      current_byte_ = bitmap[byte_offset_];
    }
  }
  bool setPosition( int64_t start_offset) { //number of bits , starts with zero
   if(start_offset < 0 || start_offset >= length_ ) return false;
    position_ = start_offset;
    byte_offset_ = start_offset / 8;
    bit_offset_ = start_offset % 8;
    current_byte_ = bitmap_[byte_offset_];
    return true;
  }

  bool IsSet() const { return (current_byte_ & (1 << bit_offset_)) != 0; }

  bool IsNotSet() const { return (current_byte_ & (1 << bit_offset_)) == 0; }

  void Next() {
    ++bit_offset_;
    ++position_;
    if (MSERF_PREDICT_FALSE(bit_offset_ == 8)) {
      bit_offset_ = 0;
      ++byte_offset_;
      if (MSERF_PREDICT_TRUE(position_ < length_)) {
        current_byte_ = bitmap_[byte_offset_];
      }
    }
  }
  void Prev() {
    --bit_offset_;
    --position_;
    if (MSERF_PREDICT_FALSE(bit_offset_ == -1)) {
      bit_offset_ = 7;
      --byte_offset_;
      if (MSERF_PREDICT_TRUE(position_ >= 0)) {
        current_byte_ = bitmap_[byte_offset_];
      }
    }
  }


  int64_t getNextSetPos() {
     if(nextSetBitInByte()){ ++bit_offset_;  return position_++;}

     //skip all zeros
     while( (position_ < length_) && (bitmap_[++byte_offset_] == 0)) { position_+= 8; }
     //print(std::cout);std::cout<<std::endl;
     if(position_ >= length_ ) return -1;

     //check the position of bit
     bit_offset_ = 0;
     current_byte_ = bitmap_[byte_offset_];
     nextSetBitInByte();  ++bit_offset_;  return position_++;
     //skip zeors
  }

  int64_t getPrevSetPos() {
     if(getPrevSetBitInByte()) { --bit_offset_;  return position_--;};

     //skip all zeros
     while(position_ >=0 && (bitmap_[--byte_offset_] == 0)) { position_-= 8; }
     if(position_ < 0 ) return -1;

     //check the position of bit
     bit_offset_ = 7;
     current_byte_ = bitmap_[byte_offset_];
     getPrevSetBitInByte();  --bit_offset_;
     return position_--;
     //skip zeors
  }
  void print(std::ostream& os) {
     std::bitset<8> val(current_byte_);
     os<<"len: " << length_ <<" pos: "<<position_ << " offset: "<<byte_offset_<< " byte: "<< val<< " bit offset: "<<bit_offset_;
  }
  void setDir(bool backword) { _backword = backword;}
  int64_t getNextPos() { if (_backword ) { return getPrevSetPos(); } return getNextSetPos();  }
  int64_t position() const { return position_; }

  int64_t length() const { return length_; }

 private:
  bool nextSetBitInByte() {
       uint8_t  bitmap = (1 << bit_offset_);
       while(bitmap && ((current_byte_& bitmap) == 0)) {bitmap<<=1; ++bit_offset_; ++position_;}
       return bitmap;
  }

  bool getPrevSetBitInByte() {
       uint8_t  bitmap = (1 << bit_offset_);
       //std::cout<<"in bitmap:"<<(uint16_t)bitmap<<std::endl;
       //print(std::cout);std::cout<<std::endl;
       while(bitmap && ((current_byte_& bitmap) == 0)) {bitmap>>=1; --bit_offset_; --position_;
       //std::cout<<"loop:"<<(uint16_t)bitmap<<std::endl;
       //print(std::cout);std::cout<<std::endl;
       }
       //std::cout<<"bitmap:"<<(uint16_t)bitmap<<std::endl;
       return bitmap;
  }

 private:
  const uint8_t* bitmap_;
  int64_t position_;
  int64_t length_;

  uint8_t current_byte_;
  int64_t byte_offset_;
  int64_t bit_offset_;

  bool _backword;
};


}   // END OF NS: util

}   // END OF NS: Mserf
#endif //__TICKER_FILES_UTIL_BITMAP_READER_H__
