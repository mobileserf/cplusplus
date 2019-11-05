#ifndef __GZSTREAM_H__
#define __GZSTREAM_H__ 1

#include <string>

class GzJoin {

public:
   GzJoin(const std::string& outputfile);
   bool append(const std::string& gzfile, bool final = false);
   void close();
   ~GzJoin() {close();}
private:
    unsigned long _crc, _tot;     /* running crc and total uncompressed length */
    FILE* _out;

};

#endif //__GZSTREAM_H__
