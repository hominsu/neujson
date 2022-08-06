//
// Created by HominSu on 2022/6/30.
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
#pragma comment(lib,"softintrin")
#endif
#endif

namespace neujson::internal {

struct DiyFp {
  static const int kDiyFpSize = 64;
  static const int kDpFractionSize = 52;
  static const int kDpExponentBias = 0x3FF + kDpFractionSize;
  static const int kDpMaxExponent = 0x7FF - kDpExponentBias;
  static const int kDpMinExponent = -kDpExponentBias;
  static const int kDpStartExponent = 1 - kDpExponentBias;
  static const uint64_t kDpExponentMask = NEUJSON_UINT64_C2(0x7FF00000, 0x00000000);
  static const uint64_t kDpFractionMask = NEUJSON_UINT64_C2(0x000FFFFF, 0xFFFFFFFF);
  static const uint64_t kDpHiddenBit = NEUJSON_UINT64_C2(0x00100000, 0x00000000);

  uint64_t f_;
  int e_;

  DiyFp() : f_(), e_() {}
  DiyFp(uint64_t _fp, int _exp) : f_(_fp), e_(_exp) {}

  explicit DiyFp(double _d) {
    union {
      double d;
      uint64_t u64;
    } u = {_d};

    int exponent = static_cast<int>((u.u64 & kDpExponentMask) >> kDpFractionSize);
    uint64_t fraction = (u.u64 & kDpFractionMask);
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
      return ::std::numeric_limits<double>::infinity();
    }
    const uint64_t exponent =
        (e_ == kDpStartExponent && (f_ & kDpHiddenBit) == 0) ? 0 : static_cast<uint64_t>(e_ + kDpExponentBias);
    u.u64 = ((f_ & kDpFractionMask) | (exponent << kDpFractionSize));
    return u.d;
  }

  DiyFp operator-(const DiyFp &_diy_fp) const {
    return {f_ - _diy_fp.f_, e_};
  }

  DiyFp operator*(const DiyFp &_diy_fp) const {
#if defined(_MSC_VER) && defined(_M_AMD64)
    uint64_t h;
    uint64_t l = _umul128(f_, _diy_fp.f_, &h);
    if (l & (uint64_t(1) << 63)) { ++h; } // rounding
    return DiyFp(h, e_ + _diy_fp.e_ + 64);
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && defined(__x86_64__)
    __extension__ typedef unsigned __int128 uint128_t;
    uint128_t p = static_cast<uint128_t>(f_) * static_cast<uint128_t>(_diy_fp.f_);
    auto h = static_cast<uint64_t>(p >> 64);
    auto l = static_cast<uint64_t>(p);
    if (l & (uint64_t(1) << 63)) { ++h; } // rounding
    return {h, e_ + _diy_fp.e_ + 64};
#else
    const uint64_t M32 = 0xFFFFFFFF;
    const uint64_t a = f_ >> 32;
    const uint64_t b = f_ & M32;
    const uint64_t c = _diy_fp.f_ >> 32;
    const uint64_t d = _diy_fp.f_ & M32;
    const uint64_t ac = a * c;
    const uint64_t bc = b * c;
    const uint64_t ad = a * d;
    const uint64_t bd = b * d;
    uint64_t tmp = (bd >> 32) + (ad & M32) + (bc & M32);
    tmp += 1U << 31;  /// mult_round
    return {ac + (ad >> 32) + (bc >> 32) + (tmp >> 32), e_ + _diy_fp.e_ + 64};
#endif
  }
};

} // namespace neujson::internal

#endif //NEUJSON_INCLUDE_NEUJSON_INTERNAL_DIY_FP_H_
