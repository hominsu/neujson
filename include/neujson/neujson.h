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

#ifndef NEUJSON_LENGTH
#define NEUJSON_LENGTH(CONST_ARRAY) (sizeof(CONST_ARRAY) / sizeof(CONST_ARRAY[0]))
#endif // NEUJSON_LENGTH

#ifndef NEUJSON_STR_LENGTH
#if defined(_MSC_VER)
#define NEUJSON_STR_LENGTH(CONST_STR) _countof(CONST_STR)
#else
#define NEUJSON_STR_LENGTH(CONST_STR) (sizeof(CONST_STR) / sizeof(CONST_STR[0]))
#endif
#endif // NEUJSON_STR_LENGTH

/*
 * @brief Avoid compiler warnings
 */
#ifndef NEUJSON_UINT64_C2
#define NEUJSON_UINT64_C2(high32, low32) ((static_cast<uint64_t>(high32) << 32) | static_cast<uint64_t>(low32))
#endif

#endif //NEUJSON_NEUJSON_NEUJSON_H_
