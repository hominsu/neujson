//
// Created by HominSu on 2022/6/28.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_INTERNAL_IEEE754_H_
#define NEUJSON_INCLUDE_NEUJSON_INTERNAL_IEEE754_H_

#include <cstdint>

#include "neujson/neujson.h"

namespace neujson::internal {

class Double {
 private:
  union {
    double d_;
    uint64_t u_;
  };

 private:
  static const int kFractionSize = 52;
  static const int kExponentBias = 0x3FF;
  static const int kStartExponent = 1 - kExponentBias;
  static const uint64_t kSignMask = NEUJSON_UINT64_C2(0x80000000, 0x00000000);
  static const uint64_t kExponentMask = NEUJSON_UINT64_C2(0x7FF00000, 0x00000000);
  static const uint64_t kFractionMask = NEUJSON_UINT64_C2(0x000FFFFF, 0xFFFFFFFF);

 public:
  Double() = default;
  explicit Double(double _d) { d_ = _d; }   // NOLINT
  explicit Double(uint64_t _u) { u_ = _u; } // NOLINT

  [[nodiscard]] double Value() const { return d_; }
  [[nodiscard]] uint64_t UInt64Value() const { return u_; }

  // @formatter:off
  [[nodiscard]] bool Sign() const { return (u_ & kSignMask) != 0; }
  [[nodiscard]] int Exponent() const { return static_cast<int>(((u_ & kExponentMask) >> kFractionSize) - kExponentBias); }
  [[nodiscard]] uint64_t Fraction() const { return u_ & kFractionMask; }
  // @formatter:on

  [[nodiscard]] bool IsNan() const { return (u_ & kExponentMask) == kExponentMask && Fraction() != 0; }
  [[nodiscard]] bool IsInf() const { return (u_ & kExponentMask) == kExponentMask && Fraction() == 0; }
  [[nodiscard]] bool IsNanOrInf() const { return (u_ & kExponentMask) == kExponentMask; }
  [[nodiscard]] bool IsNormal() const { return (u_ & kExponentMask) != 0 || Fraction() == 0; }
  [[nodiscard]] bool IsZero() const { return (u_ & (kExponentMask | kFractionMask)) == 0; }

};

} // namespace neujson::internal

#endif //NEUJSON_INCLUDE_NEUJSON_INTERNAL_IEEE754_H_
