#ifndef __MSERF_COMMON_DELEGATE_H__
#define __MSERF_COMMON_DELEGATE_H__

#include <functional>

namespace mserf {

namespace common {

#define _MEM_DELEGATES(_Q,_NAME)\
template <class T, class R, class ... P>\
struct _mem_delegate ## _NAME\
{\
    T* m_t;\
    R  (T::*m_f)(P ...) _Q;\
    _mem_delegate ## _NAME(T* t, R  (T::*f)(P ...) _Q):m_t(t),m_f(f) {}\
    R operator()(P ... p) _Q\
    {\
        return (m_t->*m_f)(std::forward<P>(p) ...);\
    }\
};\
\
template <class T, class R, class ... P>\
    _mem_delegate ## _NAME<T,R,P ...> make_delegate(T* t, R (T::*f)(P ...) _Q)\
{\
    _mem_delegate ##_NAME<T,R,P ...> d(t,f);\
    return d;\
}

_MEM_DELEGATES(,Z)
_MEM_DELEGATES(const,X)
_MEM_DELEGATES(volatile,Y)
_MEM_DELEGATES(const volatile,W)

}//end namespace common

}//end namespace mserf

#endif /* __MSERF_COMMON_DELEGATE_H__ */

#if 0 //SAMPLE USAGE
#include <iostream>
class A;
class Messaging {
      public:
      void onData(const char* msg, const A* a) {std::cout<<"inside Messaging"<<std::endl;}
};

class DMessaging {
      public:
      DMessaging() { std::cout<<"Derived cons"<<std::endl; }
      ~DMessaging() { std::cout<<"Derived destructor"<<std::endl; }
      void onData(const char* msg, const A* a) {std::cout<<"inside Derived"<<std::endl;}
};

class A{

public:
    void callback( std::function<void(const char*,  const A* a)> func) {
      _func = func;
   }
   void call() {
       const char* a="ash";
       _func(a, this);
   }
   std::function<void(const char*,  const A* a)> _func;
};

int main(int argc, char *argv[])
{

   A a;

   Messaging b;
   a.callback(mserf::common::make_delegate(&b, &Messaging::onData));
   a.call();

   DMessaging c;
   a.callback(mserf::common::make_delegate(&c, &DMessaging::onData));
   a.call();


}
#endif
