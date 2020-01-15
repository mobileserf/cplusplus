#ifndef __MSERF_MACROS_H__
#define __MSERF_MACROS_H__

#include <cstdint>

//Following codes are taken from "arrow/util/macros.h"
#define MSERF_STRINGIFY(x) #x
#define MSERF_CONCAT(x, y) x##y

#ifndef MSERF_DISALLOW_COPY_AND_ASSIGN //CONFIG
#define MSERF_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;            \
  void operator=(const TypeName&) = delete
#endif

#define MSERF_UNUSED(x) (void)x
#define MSERF_ARG_UNUSED(x)


//
// GCC can be told that a certain branch is not likely to be taken (for
// instance, a CHECK failure), and use that information in static analysis.
// Giving it this information can help it optimize for the common case in
// the absence of better information (ie. -fprofile-arcs).
//
//
#define MSERF_PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define MSERF_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#define MSERF_NORETURN __attribute__((noreturn))
#define MSERF_PREFETCH(addr) __builtin_prefetch(addr)

#define MSERF_MUST_USE_RESULT __attribute__((warn_unused_result))
#define MSERF_DEPRECATED(...) [[deprecated(__VA_ARGS__)]]
//#  define MSERF_DEPRECATED(...) __attribute__((deprecated(__VA_ARGS__)))

//from https://github.com/stgatilov/linear-vs-binary-search
#define FORCEINLINE __attribute__((always_inline)) inline
#define INLINE inline
#define NOINLINE __attribute__((noinline))

// macros to disable padding
#if !defined(MANUALLY_ALIGNED_STRUCT) //CONFIG
#define MANUALLY_ALIGNED_STRUCT(alignment) \
  _Pragma("pack(1)") struct __attribute__((aligned(alignment)))
#define STRUCT_END(name, size) \
  _Pragma("pack()") static_assert(sizeof(name) == size, "compiler breaks packing rules")
#endif

#define ALIGN(n) __attribute__((aligned(n)))


#endif //__MSERF_MACROS_H__
