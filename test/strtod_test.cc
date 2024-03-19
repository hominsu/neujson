//
// Created by Homing So on 2022/8/17.
//

#include "neujson/internal/strtod.h"

#include "gtest/gtest.h"

#if defined(__clang__)
NEUJSON_DIAG_PUSH
NEUJSON_DIAG_OFF(unreachable-code)
#elif defined(_MSC_VER)
NEUJSON_DIAG_PUSH
NEUJSON_DIAG_OFF(4127)  // conditional expression is constant
NEUJSON_DIAG_OFF(4702)  // unreachable code
#endif

using namespace neujson::internal;

TEST(strtod, check_approximation_case) {
  static const int kSignificandSize = 52;
  static const int kExponentBias = 0x3FF;
  static const uint64_t kExponentMask = NEUJSON_UINT64_C2(0x7FF00000, 0x00000000);
  static const uint64_t kSignificandMask = NEUJSON_UINT64_C2(0x000FFFFF, 0xFFFFFFFF);
  static const uint64_t kHiddenBit = NEUJSON_UINT64_C2(0x00100000, 0x00000000);

// http://www.exploringbinary.com/using-integers-to-check-a-floating-point-approximation/
// Let b = 0x1.465a72e467d88p-149
//       = 5741268244528520 x 2^-201
  union {
    double d;
    uint64_t u;
  } u{};
  u.u = 0x465a72e467d88 | ((static_cast<uint64_t>(-149 + kExponentBias)) << kSignificandSize);
  const double b = u.d;
  const uint64_t bInt = (u.u & kSignificandMask) | kHiddenBit;
  const int bExp = static_cast<int>(((u.u & kExponentMask) >> kSignificandSize) - kExponentBias - kSignificandSize);
  EXPECT_DOUBLE_EQ(1.7864e-45, b);
  EXPECT_EQ(NEUJSON_UINT64_C2(0x001465a7, 0x2e467d88), bInt);
  EXPECT_EQ(-201, bExp);

// Let d = 17864 x 10-49
  const char dInt[] = "17864";
  (void) dInt;
  const int dExp = -49;

// Let h = 2^(bExp-1)
  const int hExp = bExp - 1;
  EXPECT_EQ(-202, hExp);

  int dS_Exp2 = 0;
  int dS_Exp5 = 0;
  int bS_Exp2 = 0;
  int bS_Exp5 = 0;
  int hS_Exp2 = 0;
  int hS_Exp5 = 0;

// Adjust for decimal exponent
  if (dExp >= 0) {
    dS_Exp2 += dExp;
    dS_Exp5 += dExp;
  } else {
    bS_Exp2 -= dExp;
    bS_Exp5 -= dExp;
    hS_Exp2 -= dExp;
    hS_Exp5 -= dExp;
  }

// Adjust for binary exponent
  if (bExp >= 0)
    bS_Exp2 += bExp;
  else {
    dS_Exp2 -= bExp;
    hS_Exp2 -= bExp;
  }

// Adjust for half ulp exponent
  if (hExp >= 0)
    hS_Exp2 += hExp;
  else {
    dS_Exp2 -= hExp;
    bS_Exp2 -= hExp;
  }

// Remove common power of two factor from all three scaled values
  int common_Exp2 = (std::min)(dS_Exp2, (std::min)(bS_Exp2, hS_Exp2));
  dS_Exp2 -= common_Exp2;
  bS_Exp2 -= common_Exp2;
  hS_Exp2 -= common_Exp2;

  EXPECT_EQ(153, dS_Exp2);
  EXPECT_EQ(0, dS_Exp5);
  EXPECT_EQ(1, bS_Exp2);
  EXPECT_EQ(49, bS_Exp5);
  EXPECT_EQ(0, hS_Exp2);
  EXPECT_EQ(49, hS_Exp5);
}

#if defined(_MSC_VER) || defined(__clang__)
NEUJSON_DIAG_POP
#endif
