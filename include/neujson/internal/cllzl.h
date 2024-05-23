//
// Created by Homing So on 2022/3/13.
//

#ifndef NEUJSON_NEUJSON_INTERNAL_CLLZL_H_
#define NEUJSON_NEUJSON_INTERNAL_CLLZL_H_

#include <cstdint>

#include "neujson/neujson.h"

#if defined(_MSC_VER) && !defined(UNDER_CE)
#include <intrin.h>
#if defined(_WIN64)
#pragma intrinsic(_BitScanReverse64)
#else
#pragma intrinsic(_BitScanReverse)
#endif
#endif

namespace neujson::internal {

inline uint32_t clzll(uint64_t n) {
  NEUJSON_ASSERT(n != 0);

#if defined(_MSC_VER) && !defined(UNDER_CE)
  unsigned long r = 0;
#if defined(_WIN64)
  _BitScanReverse64(&r, n);
#else
  // scan the high 32 bits.
  if (_BitScanReverse(&r, static_cast<uint32_t>(n >> 32))) {
    return 63 - (r + 32);
  }

  // scan the low 32 bits.
  _BitScanReverse(&r, static_cast<uint32_t>(n & 0xFFFFFFFF));
#endif // _WIN64

  return 63 - r;
#elif (defined(__GNUC__) && __GNUC__ >= 4) ||                                  \
    NEUJSON_HAS_BUILTIN(__builtin_clzll)
  // __builtin_clzll wrapper
  return static_cast<uint32_t>(__builtin_clzll(n));
#else
  // naive version
  uint32_t r = 0;
  while (!(n & (static_cast<uint64_t>(1) << 63))) {
    n <<= 1;
    ++r;
  }

  return r;
#endif // _MSC_VER
}

} // namespace neujson::internal

#endif // NEUJSON_NEUJSON_INTERNAL_CLLZL_H_
