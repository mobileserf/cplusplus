#ifndef __MSERF_BIT_UTIL_FILES_UTIL_BIT_UTIL_H__
#define __MSERF_BIT_UTIL_FILES_UTIL_BIT_UTIL_H__

//CREDIT: taken from arrow-master/cpp/src/arrow/util
//
#include <cstdint>


//
// GCC can be told that a certain branch is not likely to be taken (for
// instance, a CHECK failure), and use that information in static analysis.
// Giving it this information can help it optimize for the common case in
// the absence of better information (ie. -fprofile-arcs).
//
#if defined(__GNUC__)
#define MSERF_PREDICT_FALSE(x) (__builtin_expect(!!(x), 0))
#define MSERF_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#define MSERF_NORETURN __attribute__((noreturn))
#define MSERF_NOINLINE __attribute__((noinline))
#define MSERF_PREFETCH(addr) __builtin_prefetch(addr)
#elif defined(_MSC_VER)
#define MSERF_NORETURN __declspec(noreturn)
#define MSERF_NOINLINE __declspec(noinline)
#define MSERF_PREDICT_FALSE(x) (x)
#define MSERF_PREDICT_TRUE(x) (x)
#define MSERF_PREFETCH(addr)
#else
#define MSERF_NORETURN
#define MSERF_PREDICT_FALSE(x) (x)
#define MSERF_PREDICT_TRUE(x) (x)
#define MSERF_PREFETCH(addr)
#endif

namespace Mserf {

namespace BitUtil {

// Bitmask selecting the k-th bit in a byte
static constexpr uint8_t kBitmask[] = {1, 2, 4, 8, 16, 32, 64, 128};

}   // END OF NS: BitUtil

}   // END OF NS: Mserf
#endif //__MSERF_BIT_UTIL_FILES_UTIL_BIT_UTIL_H__
