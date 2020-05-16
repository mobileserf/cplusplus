#ifndef __MSERF_COMMON_BUFREF_H__
#define __MSERF_COMMON_BUFREF_H__

#include <stdint.h>
#include <ostream>

namespace Mserf {

template<typename TYPE>
struct BufRef {
    BufRef(): _buf(nullptr), _len(0) {}
    BufRef(const char* pointer,  TYPE length): _buf(pointer), _len(length) {}
    BufRef(const BufRef& other): _buf(other._buf), _len(other._len) {}
    BufRef& operator=( const BufRef& other)
    {
      _buf = other._buf;
      _len = other._len;
      return *this;
    }
    size_t write(std::ostream& stream) {
     if(_len) {
       stream.write(_buf, _len);
       stream<<std::endl;
       stream.flush();
     }
     return _len;
   }
    void write() {
    }
    inline TYPE length() const { return _len; }
    inline const char* _buffer() const { return _buf; }

    inline bool isValuePresent()  const{ return _len? true: false;   }

    const char *_buf;
    TYPE _len;
};

using bufref16_t = Buffer<uint16_t>;

}//Mserf End namespace

#endif //__MSERF_COMMON_BUFREF_H__
