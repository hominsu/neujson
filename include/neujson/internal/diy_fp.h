//
// Created by Homing So on 2022/6/30.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_INTERNAL_DIY_FP_H_
#define NEUJSON_INCLUDE_NEUJSON_INTERNAL_DIY_FP_H_

#include <limits>

#include "cllzl.h"
#include "neujson/neujson.h"

#if defined(_MSC_VER) && defined(_M_AMD64) && !defined(__INTEL_COMPILER)
#include <intrin.h>
#if !defined(_ARM64EC_)
#pragma intrinsic(_umul128)
#else
#pragma comment(lib, "softintrin")
#endif
#endif

#ifdef __GNUC__
NEUJSON_DIAG_PUSH
NEUJSON_DIAG_OFF(effc++)
#endif // __GNUC__

namespace neujson::internal {

struct DiyFp {
  static constexpr int kDiyFpSize = 64;
  static constexpr int kDpFractionSize = 52;
  static constexpr int kDpExponentBias = 0x3FF + kDpFractionSize;
  static constexpr int kDpMaxExponent = 0x7FF - kDpExponentBias;
  static constexpr int kDpMinExponent = -kDpExponentBias;
  static constexpr int kDpStartExponent = 1 - kDpExponentBias;
  static constexpr uint64_t kDpExponentMask =
      NEUJSON_UINT64_C2(0x7FF00000, 0x00000000);
  static constexpr uint64_t kDpFractionMask =
      NEUJSON_UINT64_C2(0x000FFFFF, 0xFFFFFFFF);
  static constexpr uint64_t kDpHiddenBit =
      NEUJSON_UINT64_C2(0x00100000, 0x00000000);

  uint64_t f_;
  int e_;

  DiyFp() : f_(), e_() {}
  DiyFp(const uint64_t fp, const int exp) : f_(fp), e_(exp) {}

  explicit DiyFp(const double d) {
    union {
      double d;
      uint64_t u64;
    } const u = {d};

    const int exponent =
        static_cast<int>((u.u64 & kDpExponentMask) >> kDpFractionSize);
    const uint64_t fraction = (u.u64 & kDpFractionMask);
    if (exponent != 0) {
      f_ = fraction + kDpHiddenBit;
      e_ = exponent - kDpExponentBias;
    } else {
      f_ = fraction;
      e_ = kDpMinExponent + 1;
    }
  }

  [[nodiscard]] DiyFp Normalize() const {
    int s = static_cast<int>(clzll(f_));
    return {f_ << s, e_ - s};
  }

  [[nodiscard]] double ToDouble() const {
    union {
      double d;
      uint64_t u64;
    } u{};

    NEUJSON_ASSERT(f_ <= kDpHiddenBit + kDpFractionMask);
    if (e_ < kDpStartExponent) {
      return 0.0;
    }
    if (e_ >= kDpMaxExponent) {
      return std::numeric_limits<double>::infinity();
    }
    const uint64_t exponent =
        (e_ == kDpStartExponent && (f_ & kDpHiddenBit) == 0)
            ? 0
            : static_cast<uint64_t>(e_ + kDpExponentBias);
    u.u64 = ((f_ & kDpFractionMask) | (exponent << kDpFractionSize));
    return u.d;
  }

  DiyFp operator-(const DiyFp &diy_fp) const { return {f_ - diy_fp.f_, e_}; }

  DiyFp operator*(const DiyFp &diy_fp) const {
#if defined(_MSC_VER) && defined(_M_AMD64)
    uint64_t h;
    uint64_t l = _umul128(f_, diy_fp.f_, &h);
    if (l & (uint64_t(1) << 63)) {
      ++h;
    } // rounding
    return {h, e_ + diy_fp.e_ + 64};
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) &&              \
    defined(__x86_64__)
    __extension__ typedef unsigned __int128 uint128_t;
    uint128_t p =
        static_cast<uint128_t>(f_) * static_cast<uint128_t>(diy_fp.f_);
    auto h = static_cast<uint64_t>(p >> 64);
    auto l = static_cast<uint64_t>(p);
    if (l & (uint64_t(1) << 63)) {
      ++h;
    } // rounding
    return {h, e_ + diy_fp.e_ + 64};
#else
    constexpr uint64_t M32 = 0xFFFFFFFF;
    const uint64_t a = f_ >> 32;
    const uint64_t b = f_ & M32;
    const uint64_t c = diy_fp.f_ >> 32;
    const uint64_t d = diy_fp.f_ & M32;
    const uint64_t ac = a * c;
    const uint64_t bc = b * c;
    const uint64_t ad = a * d;
    const uint64_t bd = b * d;
    uint64_t tmp = (bd >> 32) + (ad & M32) + (bc & M32);
    tmp += 1U << 31; /// mult_round
    return {ac + (ad >> 32) + (bc >> 32) + (tmp >> 32), e_ + diy_fp.e_ + 64};
#endif
  }
};

} // namespace neujson::internal

#ifdef __GNUC__
NEUJSON_DIAG_POP
#endif // __GNUC__

#endif // NEUJSON_INCLUDE_NEUJSON_INTERNAL_DIY_FP_H_
