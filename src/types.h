#ifndef TYPE_H
#define TYPE_H
#include <cassert>
#include <cstdio>
// types

#undef FALSE
#define FALSE 0

#undef TRUE
#define TRUE 1

#ifdef _MSC_VER
#define S64_FORMAT "%I64d"
#define U64_FORMAT "%016I64X"
#else
#define S64_FORMAT "%lld"
#define U64_FORMAT "%016llX"
#endif

// macros

#ifdef _MSC_VER
#define S64(u) (u##i64)
#define U64(u) (u##ui64)
#else
#define S64(u) (u##LL)
#define U64(u) (u##ULL)
#endif

typedef signed char sint8;
typedef unsigned char uint8;
typedef signed short sint16;
typedef unsigned short uint16;
typedef signed int sint32;
typedef unsigned int uint32;

#ifdef _MSC_VER
typedef signed __int64 sint64;
typedef unsigned __int64 uint64;
#else
typedef signed long long int sint64;
typedef unsigned long long int uint64;
#endif

#ifdef DEBUG
#undef DEBUG
#define DEBUG TRUE
#else
#define DEBUG FALSE
#endif

//#define ASSERT(a) (assert(a))
#define ASSERT(a)
#endif