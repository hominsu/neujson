//
// Created by Homin Su on 2022/3/13.
//

#ifndef NEUJSON_NEUJSON_NEUJSON_H_
#define NEUJSON_NEUJSON_NEUJSON_H_

#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32)
#if defined(_WIN64) || defined(WIN64)
#define NEUJSON_ARCH_64 1
#else
#define NEUJSON_ARCH_32 1
#endif
#define NEUJSON_PLATFORM_STRING "windows"
#define NEUJSON_WINDOWS 1
#elif defined(__linux__)
#define NEUJSON_PLATFORM_STRING "linux"
#define NEUJSON_LINUX 1
#ifdef _LP64
#define NEUJSON_ARCH_64 1
#else /* _LP64 */
#define NEUJSON_ARCH_32 1
#endif /* _LP64 */
#elif defined(__APPLE__)
#define NEUJSON_PLATFORM_STRING "osx"
#define NEUJSON_APPLE 1
#ifdef _LP64
#define NEUJSON_ARCH_64 1
#else /* _LP64 */
#define NEUJSON_ARCH_32 1
#endif /* _LP64 */
#endif

#ifndef NEUJSON_WINDOWS
#define NEUJSON_WINDOWS 0
#endif
#ifndef NEUJSON_LINUX
#define NEUJSON_LINUX 0
#endif
#ifndef NEUJSON_APPLE
#define NEUJSON_APPLE 0
#endif

#if defined(__has_builtin)
#define NEUJSON_HAS_BUILTIN(x) __has_builtin(x)
#else
#define NEUJSON_HAS_BUILTIN(x) 0
#endif

#ifndef NEUJSON_ASSERT
#include <cassert>
#define NEUJSON_ASSERT(x) assert(x)
#endif // NEUJSON_ASSERT

/**
 * @brief const array length
 */
#ifndef NEUJSON_LENGTH
#define NEUJSON_LENGTH(CONST_ARRAY) (sizeof(CONST_ARRAY) / sizeof(CONST_ARRAY[0]))
#endif // NEUJSON_LENGTH

/**
 * @brief const string length
 */
#ifndef NEUJSON_STR_LENGTH
#if defined(_MSC_VER)
#define NEUJSON_STR_LENGTH(CONST_STR) _countof(CONST_STR)
#else
#define NEUJSON_STR_LENGTH(CONST_STR) (sizeof(CONST_STR) / sizeof(CONST_STR[0]))
#endif
#endif // NEUJSON_STR_LENGTH

// stringification
#define NEUJSON_STRINGIFY(X) NEUJSON_DO_STRINGIFY(X)
#define NEUJSON_DO_STRINGIFY(X) #X

// concatenation
#define NEUJSON_JOIN(X, Y) NEUJSON_DO_JOIN(X, Y)
#define NEUJSON_DO_JOIN(X, Y) X##Y

/**
 * @brief adopted from Boost
 */
#define NEUJSON_VERSION_CODE(x,y,z) (((x)*100000) + ((y)*100) + (z))

/**
 * @brief gnuc version
 */
#if defined(__GNUC__)
#define NEUJSON_GNUC \
    NEUJSON_VERSION_CODE(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#endif

#if defined(__clang__) || (defined(NEUJSON_GNUC) && NEUJSON_GNUC >= NEUJSON_VERSION_CODE(4,2,0))

#define NEUJSON_PRAGMA(x) _Pragma(NEUJSON_STRINGIFY(x))
#if defined(__clang__)
#define NEUJSON_DIAG_PRAGMA(x) NEUJSON_PRAGMA(clang diagnostic x)
#else
#define NEUJSON_DIAG_PRAGMA(x) NEUJSON_PRAGMA(GCC diagnostic x)
#endif
#define NEUJSON_DIAG_OFF(x) NEUJSON_DIAG_PRAGMA(ignored NEUJSON_STRINGIFY(NEUJSON_JOIN(-W,x)))

// push/pop support in Clang and GCC>=4.6
#if defined(__clang__) || (defined(NEUJSON_GNUC) && NEUJSON_GNUC >= NEUJSON_VERSION_CODE(4,6,0))
#define NEUJSON_DIAG_PUSH NEUJSON_DIAG_PRAGMA(push)
#define NEUJSON_DIAG_POP  NEUJSON_DIAG_PRAGMA(pop)
#else // GCC >= 4.2, < 4.6
#define NEUJSON_DIAG_PUSH /* ignored */
#define NEUJSON_DIAG_POP /* ignored */
#endif

#elif defined(_MSC_VER)

// pragma (MSVC specific)
#define NEUJSON_PRAGMA(x) __pragma(x)
#define NEUJSON_DIAG_PRAGMA(x) NEUJSON_PRAGMA(warning(x))

#define NEUJSON_DIAG_OFF(x) NEUJSON_DIAG_PRAGMA(disable: x)
#define NEUJSON_DIAG_PUSH NEUJSON_DIAG_PRAGMA(push)
#define NEUJSON_DIAG_POP  NEUJSON_DIAG_PRAGMA(pop)

#else

#define NEUJSON_DIAG_OFF(x) /* ignored */
#define NEUJSON_DIAG_PUSH   /* ignored */
#define NEUJSON_DIAG_POP    /* ignored */

#endif

/*
 * @brief Avoid compiler warnings
 */
#ifndef NEUJSON_UINT64_C2
#define NEUJSON_UINT64_C2(high32, low32) ((static_cast<uint64_t>(high32) << 32) | static_cast<uint64_t>(low32))
#endif

#endif //NEUJSON_NEUJSON_NEUJSON_H_
