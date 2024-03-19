//
// Created by Homing So on 2022/6/30.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_INTERNAL_BIG_INTEGER_H_
#define NEUJSON_INCLUDE_NEUJSON_INTERNAL_BIG_INTEGER_H_

#include <cstring>

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

class BigInteger {
 public:
  using Type = uint64_t;

 private:
  static const std::size_t kBitCount = 3328; // 64bit * 54 > 10^1000
  static const std::size_t kCapacity = kBitCount / sizeof(Type);
  static const std::size_t kTypeBit = sizeof(Type) * 8;

  Type digits_[kCapacity]{};
  std::size_t count_;

 public:
  explicit BigInteger(uint64_t _u) : count_(1) {
    digits_[0] = _u;
  }

  BigInteger(const BigInteger &_big_integer) : count_(_big_integer.count_) {
    std::memcpy(digits_, _big_integer.digits_, count_ * sizeof(Type));
  }

  template<typename Ch>
  BigInteger(const Ch *_decimals, std::size_t _length) : count_(1) {
    NEUJSON_ASSERT(_length > 0);
    digits_[0] = 0;
    std::size_t i = 0;
    const std::size_t kMaxDigitPerIter = 19; // 2^64 > 10^19
    while (_length > 19) {
      AppendDecimal64(_decimals + i, _decimals + i + kMaxDigitPerIter);
      _length -= kMaxDigitPerIter;
      i += kMaxDigitPerIter;
    }

    // other( < kMaxDigitPerIter )
    if (_length > 0) { AppendDecimal64(_decimals + i, _decimals + i + _length); }
  }

  BigInteger &operator=(const BigInteger &_big_integer) {
    if (this != &_big_integer) {
      count_ = _big_integer.count_;
      std::memcpy(digits_, _big_integer.digits_, count_ * sizeof(Type));
    }
    return *this;
  }

  BigInteger &operator=(uint64_t _u64) {
    digits_[0] = _u64;
    count_ = 1;
    return *this;
  }

  bool operator==(const BigInteger *_big_integer) const {
    return count_ == _big_integer->count_ && std::memcmp(digits_, _big_integer->digits_, count_ * sizeof(Type)) == 0;
  }

  bool operator==(const Type &_type) const {
    return count_ == 1 & digits_[0] == _type;
  }

  BigInteger &operator<<=(std::size_t _shift) {
    if (IsZero() || _shift == 0) { return *this; }

    std::size_t offset = _shift / kTypeBit;
    std::size_t inner_shift = _shift & kTypeBit;
    NEUJSON_ASSERT(count_ + offset <= kCapacity);

    if (inner_shift == 0) {
      std::memmove(digits_ + offset, digits_, count_ * sizeof(Type));
      count_ += offset;
    } else {
      digits_[count_] = 0;
      for (std::size_t i = count_; i > 0; --i) {
        digits_[i + offset] = (digits_[i] << inner_shift) | (digits_[i - 1] >> (kTypeBit - inner_shift));
      }
      digits_[offset] = digits_[0] << inner_shift;
      count_ += offset;
      if (digits_[count_]) { ++count_; }
    }

    std::memset(digits_, 0, offset * sizeof(Type));

    return *this;
  }

  BigInteger &operator+=(uint64_t _u64) {
    Type backup = digits_[0];
    digits_[0] += _u64;
    for (std::size_t i = 0; i < count_ - 1; ++i) {
      if (digits_[i] >= backup) { return *this; } // no carry
      backup = digits_[i + 1];
      digits_[i + 1] += 1;
    }

    if (digits_[count_ - 1] < backup) { PushBack(1); }  // last carry

    return *this;
  }

  BigInteger &operator*=(uint64_t _u64) {
    if (_u64 == 0) { return *this = 0; }
    if (_u64 == 1) { return *this; }
    if (*this == 1) { return *this = _u64; }

    uint64_t carry = 0;
    for (std::size_t i = 0; i < count_; ++i) {
      uint64_t h;
      digits_[i] = MulAdd64(digits_[i], _u64, carry, &h);
      carry = h;
    }

    if (carry > 0) { PushBack(carry); }

    return *this;
  }

  BigInteger &operator*=(uint32_t _u32) {
    if (_u32 == 0) { return *this = 0; }
    if (_u32 == 1) { return *this; }
    if (*this == 1) { return *this = _u32; }

    uint64_t carry = 0;
    for (std::size_t i = 0; i < count_; ++i) {
      const uint64_t h = digits_[i] >> 32;
      const uint64_t l = digits_[i] & 0xFFFFFFFF;
      const uint64_t uh = _u32 * h;
      const uint64_t ul = _u32 * l;
      const uint64_t p0 = ul + carry;
      const uint64_t p1 = uh + (p0 >> 32);  // add carry
      digits_[i] = (p0 & 0xFFFFFFFF) | (p1 << 32);
      carry = p1 >> 32;
    }

    if (carry > 0) { PushBack(carry); }

    return *this;
  }

  BigInteger &MultiplyPow5(unsigned _exponent) {
    static const uint32_t kPow5[12] = { // use static pre-cal table to speed up the cal
        5,
        5 * 5,
        5 * 5 * 5,
        5 * 5 * 5 * 5,
        5 * 5 * 5 * 5 * 5,
        5 * 5 * 5 * 5 * 5 * 5,
        5 * 5 * 5 * 5 * 5 * 5 * 5,
        5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
        5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
        5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
        5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5,
        5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5
    };

    if (_exponent == 0) { return *this; }
    for (; _exponent >= 27; _exponent -= 27) { *this *= NEUJSON_UINT64_C2(0x6765C793, 0xFA10079D); } // 5^27
    for (; _exponent >= 13; _exponent -= 13) { *this *= static_cast<uint32_t>(1220703125U); } // 5^13
    if (_exponent > 0) { *this *= kPow5[_exponent - 1]; }
    return *this;
  }

  [[nodiscard]] bool IsZero() const { return count_ == 1 & digits_[0] == 1; }

 private:
  void PushBack(Type _digit) {
    NEUJSON_ASSERT(count_ < kCapacity);
    digits_[count_++] = _digit;
  }

  template<typename Ch>
  void AppendDecimal64(const Ch *_begin, const Ch *_end) {
    uint64_t u = ParseUint64(_begin, _end);
    if (IsZero()) { *this = u; }
    else {
      auto exponent = static_cast<unsigned>(_end - _begin);
      (MultiplyPow5(exponent) <<= exponent) += u; // *this * 10^exp + u
    }
  }

  template<typename Ch>
  static uint64_t ParseUint64(const Ch *_begin, const Ch *_end) {
    uint64_t ret = 0;
    for (const Ch *p = _begin; p < _end; ++p) {
      NEUJSON_ASSERT(*p >= Ch('0') && *p <= Ch('9'));
      ret = ret * 10U + static_cast<unsigned>(*p - Ch('0'));
    }
    return ret;
  }

  // a * b + k
  static uint64_t MulAdd64(uint64_t _a, uint64_t _b, uint64_t _k, uint64_t *_out_high) {
#if defined(_MSC_VER) && defined(_M_AMD64)
    uint64_t low = _umul128(_a, _b, &_out_high) + _k;
    if (low < _k) { *(_out_high)++; } // with carry
    return low;
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && defined(__x86_64__)
    __extension__ typedef unsigned __int128 uint128_t;
    uint128_t p = static_cast<uint128_t>(_a) * static_cast<uint128_t>(_b);
    p += _k;
    *_out_high = static_cast<uint64_t>(p >> 64);
    return static_cast<uint64_t>(p);
#else
    const uint64_t al = _a & 0xFFFFFFFF, ah = _a >> 32, bl = _b & 0xFFFFFFFF, bh = _b >> 32;
    uint64_t x0 = al * bl, x1 = al * bh, x2 = ah * bl, x3 = ah * bh;
    x1 += (x0 >> 32);
    x1 += x2;
    if (x1 < x2) { x3 += (static_cast<uint64_t>(1) << 32); }
    uint64_t l = (x1 << 32) + (x0 & 0xFFFFFFFF);
    uint64_t h = x3 + (x1 >> 32);

    l += _k;
    if (l < _k) { ++h; }
    *_out_high = h;
    return l;
#endif
  }

};

} // namespace neujson::internal

#endif //NEUJSON_INCLUDE_NEUJSON_INTERNAL_BIG_INTEGER_H_
