#include "common/bit/BitmapWriter.h"
#include "common/bitBit/mapReader.h"
#include "common/ConsoleColor.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <bitset>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace Mserf;
using namespace Mserf::BitUtil;

uint8_t val1 = 0b10001110;
uint8_t val2 = 0b00101010;
std::vector<int> gInput = {0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1};
bool only_error_log = false;

template<typename T>
void displayResult(T& data) {
   std::cout<<data;
   std::cout<<std::endl;
}

template<typename T, typename... Args>
void displayResult(T& data,  Args ... args) {
   std::cout<<data<<" ";
   displayResult(args...);
}

template<typename T, typename ... Args>
void log(T& displaystring, Args ... args) {
   displayResult(displaystring, std::forward<Args>(args)...);
}

template<typename T, typename ... Args>
void log_info(T& displaystring, Args ... args) {
   if(!only_error_log) {
     log(displaystring, std::forward<Args>(args)...);
   }
}

template<typename T, typename ... Args>
void log_start(T& displaystring, Args ... args) {
   if(!only_error_log) {
     std::cout<<std::endl;
     displayResult(displaystring, std::forward<Args>(args)...);
   }
}

template <class BitmapWriter>
void WriteVectorToWriter(BitmapWriter& writer, const std::vector<int> values) {
  for (const auto& value : values) {
    if (value) {
      writer.Set();
    } else {
      writer.Clear();
    }
    writer.Next();
  }
  std::cout<<std::endl;
  writer.Finish();
}

void WriteToVector(BitmapReader& reader, size_t numberof_bits,  std::vector<int>& values) {

  values.resize(numberof_bits, 0);
  for (auto i = 0 ; i < numberof_bits; ++i ) {
     if(reader.IsSet()) {
       values[i] = 1;
     }
     reader.Next();
  }
}

void WriteToVectorReverse(BitmapReader& reader, size_t numberof_bits,  std::vector<int>& values) {

  values.resize(numberof_bits, 0);
  std::cout<<std::endl;

  for (int64_t i = numberof_bits-1 ; i >= 0; --i ) {
#if 0
     std::cout<<i<< ".  ";
     reader.print(std::cout);
     std::cout<<std::endl;
#endif
     if(reader.IsSet()) {
       values[i] = 1;
     }
     reader.Prev();
  }
}

void WriteToVectorFast(BitmapReader& reader, size_t numberof_bits,  std::vector<int>& values) {

  values.resize(numberof_bits, 0);
  int64_t pos = 0;
  //reader.print(std::cout);
  //std::cout<<std::endl;
  while( (pos=reader.getNextSetPos()) != -1) {
#if 0
       reader.print(std::cout);
       std::cout<<" set pos: "<<pos<<std::endl;
#endif
       values[pos] = 1;
  }
}

void WriteToVectorReverseFast(BitmapReader& reader, size_t numberof_bits,  std::vector<int>& values) {


  values.resize(numberof_bits, 0);
  int64_t pos = 0;
 //reader.print(std::cout);
 //std::cout<<std::endl;
  while( (pos=reader.getPrevSetPos()) != -1) {
#if 0
       //reader.print(std::cout);
       std::cout<<" set pos: "<<pos<<std::endl;
#endif
       values[pos] = 1;
  }
}


void compareVector(const std::vector<int>& input, const std::vector<int>& output) {
  if(input.size() != output.size()) {
    std::cout<<"input size: "<<input.size() << " and output size: "<<output.size()<< " are differ"<<std::endl;
  }
   std::cout<<"inp: ";
   std::copy(input.begin(), input.end(), std::ostream_iterator<int>(std::cout, " "));
   std::cout<<std::endl;
   std::cout<<"out: ";
   std::copy(output.begin(), output.end(), std::ostream_iterator<int>(std::cout, " "));
   std::cout<<std::endl;
}

template<typename T, typename ... Args>
bool writeResult( bool result,  T& displaystring, Args ... args) {
   //bool result = displayResult( output == output, disply);
   if(  result) {
     if(!only_error_log) {
       std::cout<<green<<"SUCCESS: ";
       displayResult(displaystring, std::forward<Args>(args)...);
     }
   } else {
     std::cout<<red<<"FAILED: ";
     displayResult(displaystring, std::forward<Args>(args)...);
   }
   std::cout<<clear;
   return result;
}

template<typename T, typename ... Args>
bool compareVector(const std::vector<int>& input, const std::vector<int>& output,  T& displaystring, Args ... args) {
   //bool result = displayResult( output == output, disply);
   bool result = (input == output);
   writeResult(result, displaystring, std::forward<Args>(args)...);
   if(!result)
     compareVector(input, output);
   return result;
}

void BitmapFromVector(const std::vector<int>& values,  uint8_t* out_buffer) { /*outbuffer must have enough memory*/
  const int64_t length = values.size();
  auto writer = BitmapWriter(out_buffer, 0, length);
  WriteVectorToWriter(writer, values);
}


std::shared_ptr<uint8_t> BitmapFromVector(const std::vector<int>& values) { /*outbuffer must have enough memory*/
  const int64_t length = values.size();
  size_t buffer_size = (length+7)/2;
  std::shared_ptr<uint8_t> buffer = std::make_shared<uint8_t>(buffer_size);
  uint8_t* out_buffer = buffer.get();
  out_buffer[buffer_size-1] = 0; //set last value to zero
  auto writer = BitmapWriter(out_buffer, 0, length);
  WriteVectorToWriter(writer, values);
  return buffer;
}

bool compareBytearray(std::vector<int>& input, uint8_t* val) {
   auto inputSize = input.size();
   size_t inputByte = (inputSize +7)/8;

   for(auto i = 0; i <inputByte; ++i) {
       std::bitset<8> bits(val[i]);
       for(auto j = i*8, k =0 ; (k< 8) && (j < inputSize);  ++j, ++k) {
         if(bits[k] != input[j]) {
           return false;
         }
       }
   }
   return true;
}

void logBytearray(std::vector<int>& input, uint8_t* val) {
   std::cout<<"inp: ";
   std::copy(input.begin(), input.end(), std::ostream_iterator<int>(std::cout, " "));
   std::cout<<std::endl;


   std::cout<<"out: ";
   for(auto i = 0; i <2; ++i) {
     std::bitset<8> bits(val[i]);
      for(auto j = i*8, k =0 ; (k< 8) && (j < input.size()); ++j, ++k) std::cout<<bits[k]<<" ";
  }
  std::cout<<std::endl;
}

bool checkWriteResult(std::vector<int>& input, uint8_t* val) {


   bool result = compareBytearray(input, val);

   writeResult(result, __func__);

   if(!result) { logBytearray(input, val); }

   return result;
}

bool testwtiter() {

   log_start("starting ", __func__);
   std::vector<int> input(gInput);
   std::shared_ptr<uint8_t> buffer =  BitmapFromVector(input);
   uint8_t* val = buffer.get();
#if 0
   std::bitset<8> val1(val[0]);
   std::bitset<8> val2(val[1]);
   std::cout<<val1 <<" " << val2<<std::endl;
#endif
   //if((val[0] == 0b01110001) && (val[1] == 0b01010100)) {
   bool result = checkWriteResult(input, val);

   //std::vector<uint32_t> vec
   return result;
}

bool testReader() {

   log_start("starting ", __func__);
   uint8_t val[2];
   val[0] = val1; val[1] = val2;
   std::vector<int> output ;
   std::vector<int> resultV(gInput);
   BitmapReader reader(val, 0, resultV.size());
   WriteToVector(reader, resultV.size(), output);
   return compareVector(resultV, output, "testReader");
}

bool testWriteAndRead() {
   //write
   log_start("starting ", __func__);
   std::vector<int> input(gInput);
   std::shared_ptr<uint8_t> buffer =  BitmapFromVector(input);
   uint8_t* val = buffer.get();

   //read data
   std::vector<int> output ;
   BitmapReader reader(val, 0, input.size());
   WriteToVector(reader, input.size(), output);
   return compareVector(input, output, __func__);

}
bool testReaderReverse() {

   log_start("starting ", __func__);
   uint8_t val[2];
   val[0] = val1; val[1] = val2;
   std::vector<int> output ;
   std::vector<int> resultV(gInput);
   BitmapReader reader(val, resultV.size()-1, resultV.size());
   WriteToVectorReverse(reader, resultV.size(), output);
   return compareVector(resultV, output, __func__);
}

bool testReaderFast() {

   log_start("starting ", __func__);
   uint8_t val[2];
   val[0] = val1; val[1] = val2;
   std::vector<int> output ;
   std::vector<int> resultV(gInput);
   BitmapReader reader(val, 0, resultV.size());
   WriteToVectorFast(reader, resultV.size(), output);
   return compareVector(resultV, output, "testReaderFast");
}

bool testReaderReverseFast() {

   log_start("starting ", __func__);
   uint8_t val[2];
   val[0] = val1; val[1] = val2;
   std::vector<int> output ;
   std::vector<int> resultV(gInput);
   BitmapReader reader(val, resultV.size()-1, resultV.size());
   WriteToVectorReverseFast(reader, resultV.size(), output);
   return compareVector(resultV, output, __func__);
}

#define MAX_VEC_SIZE 15
bool testReaderFast(size_t pos) {

   log_start("starting ", __func__);
   //if (pos>MAX_VEC_SIZE ) return false;
   bool result  = false;
   uint8_t val[2];
   val[0] = val1; val[1] = val2;
   std::vector<int> output ;
   // same as  testReaderFast, but read from 4th position
   std::vector<int> resultV(gInput);
   if(pos >=  resultV.size()) {
     //out of position test;
     auto tmp = resultV.size();
     resultV.clear();
     resultV.resize(tmp, 0);
   }
   else {
     for(auto i = 0; i < pos; ++i ) resultV[i] = 0;

   }
   BitmapReader reader(val, pos, resultV.size()); //set start position as 3 (index start with 0)
   WriteToVectorFast(reader, resultV.size(), output);
   return compareVector(resultV, output, "testReader: pos", pos);

   return result;
}

bool testRandomRead() {

   log_start("starting ", __func__);
   uint8_t val[2];
   val[0] = val1; val[1] = val2;
   std::vector<int> output ;
   std::vector<int> resultV(gInput);
   BitmapReader reader(val, 0, resultV.size());
   WriteToVector(reader, resultV.size(), output);
   bool result =  compareVector(resultV, output, "testReader");
   if(!result) return result;

   /* initialize random seed: */
   srand (time(NULL));
   for (auto i = 0; i< 100;++i) {
    auto pos  = rand() % resultV.size();

    reader.setPosition(pos);
    auto dir  = rand() % 2;

    int64_t index = 0;
    if(dir) {
      while( (index=reader.getNextSetPos()) != -1) { output[index] = 1; }
    }
    else {
      while( (index=reader.getPrevSetPos()) != -1) { output[index] = 1; }
    }
    result  = compareVector(resultV, output, __func__, "start index:",  pos, "dir:", dir);
    if(!result) {
      std::cout<<red;
      reader.print(std::cout);
      std::cout<<std::endl;
      std::cout<<clear;
      return result;
    }
   }
   return result;
}

bool testReaderReverseFast(size_t pos) {

   log_start("starting ", __func__);
   //if (pos>MAX_VEC_SIZE ) return false;
   bool result  = false;
   uint8_t val[2];
   val[0] = val1; val[1] = val2;
   std::vector<int> output ;
   // same as  testReaderFast, but read from 4th position
   std::vector<int> resultV(gInput);
   for(auto i = pos+1; i < resultV.size(); ++i ) resultV[i] = 0;
   if(pos>=  resultV.size()) {
     //out of position test;
     auto tmp = resultV.size();
     resultV.clear();
     resultV.resize(tmp, 0);
   }

   BitmapReader reader(val, pos, resultV.size()); //set start position as 3 (index start with 0)
   WriteToVectorReverseFast(reader, resultV.size(), output);
   return compareVector(resultV, output, __func__, ":pos", pos);

   return result;
}

bool testAllPos() {
  bool result = false;
  for (auto i = 0; i < gInput.size(); ++i) {
    log_info("Testing pos: ", i);
    result = testReaderFast(i);
    if(!result)  std::cout<<red<<"Failed at pos: "<< i<<clear<<std::endl;
  }
  return result;
}

bool testAllPosReverse() {
  bool result = false;
  for (auto i = 0; i < gInput.size(); ++i) {
    log_info("Testing pos: ", i);
    //std::cout<<"Testing pos: "<< i<<std::endl;
    result = testReaderReverseFast(i);
    if(!result)  std::cout<<"Failed at pos: "<< i<<std::endl;
  }
  return result;
}

bool testbitmap() {
   bool result = testwtiter();
   if(!testReader() &&  result) {result = false; }
   if(!testWriteAndRead() &&  result) {result = false; }
   if(!testReaderFast() &&  result) {result = false; }
   if(!testAllPos() &&  result) {result = false; }
   if(!testReaderReverse() &&  result) {result = false; }
   if(!testReaderReverseFast() &&  result) {result = false; }
   if(!testAllPosReverse() &&  result) {result = false; }
   if(!testRandomRead() &&  result) {result = false; }
   return result;
}

bool flipGlobal() {

  log_start(__func__);
  val1 ^= 0xFF;
  val2 ^= 0xFF;
  val2 &= 0x3F; //don't change last 2 digit

  for(auto& result: gInput)
     result ^= 1;
}

int main(int argc, char * argv[]) {

  bool result  = false;

  //only_error_log = true;
  result = testbitmap();

  only_error_log = false;
  log_info("reversing bit") ;
  flipGlobal();
  if(!testbitmap() &&  result) {result = false; }
  return result? 0:-1;

}
