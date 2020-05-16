#ifndef __COMMON_ENCODE_DECODE_H__
#define __COMMON_ENCODE_DECODE_H__

#include <cstring>


#define ENCODE_START(_buf) \
     uint8_t* _etmp  = _buf;

#define ENCODE_LENGTH(_buf) \
      (_etmp - _buf)

#define DECODE_START(_buf) \
     uint8_t* _dtmp  = _buf;

#define DECODE_LENGTH(_buf) \
      (_dtmp - _buf)

#if 0
#define ENCODE_FUNC_START(_func_name) \
   size_t _func_name(uint8_t* buf) { \
   ENCODE_START(buf);

#define ENCODE_FUNC_END \
      return ENCODE_LENGTH(buf);
#endif

#define ENCODE_ADVANCE_BUFFER(_len) \
   _etmp += _len;

#define DECODE_ADVANCE_BUFFER(_len) \
   _dtmp += _len;

#define ENCODE_BASIC_TYPE(_var_name) \
     *((decltype(_var_name)*)_etmp) = _var_name;  _etmp += sizeof(decltype(_var_name));

#define DECODE_BASIC_TYPE(_var_name) \
     _var_name = *((decltype(_var_name)*)_dtmp) ; _dtmp += sizeof(decltype(_var_name));

#define ENCODE_AS_TYPE(_var_name, _type) \
     *((_type*)_etmp) = static_cast<_type>(_var_name);  _etmp += sizeof(_type);

#define DECODE_AS_TYPE(_var_name, _type) \
     _var_name = static_cast<decltype(_var_name)>(*((_type*)_dtmp)) ; _dtmp += sizeof(_type);

#define ENCODE_ENUM_UINT8(_var_name) \
     ENCODE_AS_TYPE(_var_name, uint8_t)

#define ENCODE_ENUM_UINT16(_var_name) \
     ENCODE_AS_TYPE(_var_name, uint16_t)

#define DECODE_ENUM_UINT8(_var_name) \
     DECODE_AS_TYPE(_var_name, uint8_t)

#define DECODE_ENUM_UINT16(_var_name) \
     DECODE_AS_TYPE(_var_name, uint16_t)

#define ENCODE_BYTE_ARRAY(_var_name, len) \
    std::memcpy(_etmp, _var_name, len); _etmp+= len;

#define DECODE_BYTE_ARRAY(_var_name, len) \
    std::memcpy(_var_name, _dtmp, len); _dtmp+= len;

#define ENCODE_CHAR_ARRAY(_var_name) \
    size_t len = strlen(_var_name) +1; \
    std::memcpy(_etmp, _var_name, len); _etmp+= len;

#define DECODE_CHAR_ARRAY(_var_name) \
    strcpy(_var_name, (char*)_dtmp); _dtmp+= strlen(_var_name)+1;

#define ENCODE_STRING(_var_name) \
    ENCODE_BYTE_ARRAY(_var_name.c_str(), _var_name.length())

#define DECODE_STRING(_var_name, len) \
    _var_name.assign(reinterpret_cast<char*>(_dtmp), len); _dtmp += len;

#define PACK_BASIC(_buf, _var_name, _type, len) \
     *((type*)(_buf-len) = (type)_var_name;

#define PACK_UINT8(_buf, _var_name) \
     PACK_BASIC(_buf, _var_name, uint8_t, 0)

#define PACK_UINT16(_buf, var_name, _type) \
     PACK_BASIC(_buf, _var_name, uint16_t, 0)

#define PACK_UINT32(_buf, var_name, _type) \
     PACK_BASIC(_buf, _var_name, uint32_t, 0)

#if 0
struct EncodeDecodeEx {

  enum eCompressionAlgo {
    NONE = 0,
    GZIP = 1,
    LZ4 = 2,
    TXT = 3,
    BINARY = 4
  };
  uint8_t _data;
  std::string _str;
  char* _arr;
  eCompressionAlgo _algo;

  size_t encodeHeader(uint8_t* buf) {
     ENCODE_START(buf);
     ENCODE_BASIC_TYPE(_data)
     ENCODE_STRING(_str)
     ENCODE_BYTE_ARRAY(_arr,1)
     ENCODE_ENUM_UINT8(_algo);
     return ENCODE_LENGTH(buf);
  }
  size_t decodeHeader(uint8_t* buf) {
     DECODE_START(buf);
     DECODE_BASIC_TYPE(_data)
     DECODE_STRING(_str, 1)
     DECODE_BYTE_ARRAY(_arr,1)
     DECODE_ENUM_UINT8(_algo);
     return DECODE_LENGTH(buf);
  }


};
#endif

#endif  // COMMON_ENCODE_DECODE
