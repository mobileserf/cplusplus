#ifndef __MSERF_COMMON_BUFFER_H__
#define __MSERF_COMMON_BUFFER_H__

#include "common/EncodeDecode.h"
#include <fstream>
#include <stdint.h>
#include <assert.h>

namespace Mserf {

template<typename TYPE , size_t ARRAY_SIZE> struct bufferT {
    bufferT(): len(0) {}
    bufferT(const uint8_t* pointer,  TYPE length){ copy(pointer, length); }
    bufferT(char* pointer,  TYPE length)         { copy(reinterpret_cast<uint8_t*>(pointer), length);}
    bufferT(const bufferT& other)                { copy(other.buf, other.len);  }
    bufferT& operator=( const bufferT& other)
    {
      if(this != &other) { copy(other.buf, other.len); }
      return *this;
    }
    ~bufferT() {  clear();  }
    void clear() {  len = 0;  }
    void copy(const uint8_t* buffer,  TYPE length) {
       assert(length<=ARRAY_SIZE);
       len = length;
      DECODE_START(const_cast<uint8_t*>(buffer));
      DECODE_BYTE_ARRAY(buf,len)
    }
    void writeData(std::ostream& stream) {
     if(len) {
       stream.write(reinterpret_cast<char *>(buf), len);
     }
    }
    void writeLen(std::ostream& stream) {
     stream.write(reinterpret_cast<const char *>(&(len)), sizeof(TYPE));
    }
    void write(std::ostream& stream) {
     writeLen(stream);
     writeData(stream);
    }
    void readData(std::istream& stream) {
      //make sure you fill len before calling this
      if(len) {
        stream.read(reinterpret_cast<char *>(buf) , len);
      }
    }
    void readLen(std::istream& stream) {
      stream.read(reinterpret_cast<char *>(&(len)), sizeof(TYPE));
    }
    void read(std::istream& stream) {
      //char a[sizeof(T)];
      readLen(stream);
      readData(stream);
    }
    void seek_end(std::istream& is) {
       //skip the reeam
      if(len) {
         is.seekg(len, is.cur);
      }

    }
    size_t encode(uint8_t* buffer) const {
     ENCODE_START(buffer);
     ENCODE_BASIC_TYPE(len)
     ENCODE_BYTE_ARRAY(buf,len)
     return ENCODE_LENGTH(buffer);
    }
    //ATTENTION, buf should have enough space, avoid using this function
    size_t decode(uint8_t* buffer) {
      DECODE_START(buffer);
      DECODE_BASIC_TYPE(len)
      assert(len<=ARRAY_SIZE);
      //char* tmp = const_cast<char*>(buf);
      DECODE_BYTE_ARRAY(buf,len)
      return DECODE_LENGTH(buffer);
    }
    size_t decodeString(uint8_t* buffer) {
      size_t size = decode(buffer);
      buf[len]='\0';
      return size;
    }

    inline TYPE length() const { return len; }
    inline const uint8_t* buffer() const { return buf; }

    TYPE len;
    uint8_t buf[ARRAY_SIZE];
};

template<typename TYPE> struct bufferT<TYPE, 0> {
    bufferT(): buf(NULL), len(0) {}

    bufferT(const uint8_t* pointer,  TYPE length): buf(pointer), len(length) {} /* we own the buffer, don't delete it */
    bufferT(char* pointer,  TYPE length): buf(reinterpret_cast<uint8_t*>(pointer)), len(length) {} /* we own the buffer, don't delete it */
    bufferT(const bufferT& other)
    {
       copy(other.buf, other.len);
    }
    bufferT& operator=( const bufferT& other)
    {
      if(this != &other) { copy(other.buf, other.len); }
      return *this;
    }
    ~bufferT() {
      clear();
    }
    void clear() {
      if(buf)  {
        delete buf;
        buf = nullptr;
      }
      len = 0;
    }
    void copy(const uint8_t* pointer,  TYPE length) {
       len = length;
       if(buf) { delete buf;  buf = nullptr; }
       if(length) {
         buf = new uint8_t[len];
         memcpy(buf, pointer, len);
       }
    }
    void writeData(std::ostream& stream) {
     if(len) {
       stream.write(reinterpret_cast<char *>(buf), len);
     }
    }
    void writeLen(std::ostream& stream) {
     stream.write(reinterpret_cast<const char *>(&(len)), sizeof(TYPE));
    }
    void write(std::ostream& stream) {
     writeLen(stream);
     writeData(stream);
    }
    void readData(std::istream& stream) {
      //make sure you fill len before calling this
      if(len) {
        if(nullptr == buf) {
           buf = new uint8_t[len]; //caller shoud remove this
        }
        stream.read(reinterpret_cast<char *>(buf) , len);
      }
    }
    void readLen(std::istream& stream) {
      stream.read(reinterpret_cast<char *>(&(len)), sizeof(TYPE));
    }
    void read(std::istream& stream) {
      //char a[sizeof(T)];
      readLen(stream);
      readData(stream);
    }
    void seek_end(std::istream& is) {
       //skip the reeam
      if(len) {
         is.seekg(len, is.cur);
      }

    }
    size_t encode(uint8_t* buffer) const {
     ENCODE_START(buffer);
     ENCODE_BASIC_TYPE(len)
     ENCODE_BYTE_ARRAY(buf,len)
     return ENCODE_LENGTH(buffer);
    }
    //ATTENTION, buf should have enough space, avoid using this function
    size_t decode(uint8_t* buffer) {
      DECODE_START(buffer);
      DECODE_BASIC_TYPE(len)
      //char* tmp = const_cast<char*>(buf);
      DECODE_BYTE_ARRAY(buf,len)
      return DECODE_LENGTH(buffer);
    }
    size_t decodeString(uint8_t* buffer) {
      size_t size = decode(buffer);
      buf[len]='\0';
      return size;
    }

    inline TYPE length() const { return len; }
    inline const uint8_t* buffer() const { return buf; }

    uint8_t *buf;
    TYPE len;
};

using bufptr8_t = bufferT<uint8_t, 0>;
using bufptr16_t = bufferT<uint16_t,0>;
using bufptr32_t = bufferT<uint32_t, 0>;
using bufptr64_t = bufferT<uint64_t, 0>;

} //end namespace Mserf
#endif //__MSERF_COMMON_BUFFER_H__
