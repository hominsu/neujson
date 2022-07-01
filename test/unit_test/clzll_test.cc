//
// Created by HominSu on 2022/7/1.
//

#include "unit_test.h"
#include "neujson/internal/cllzl.h"

using namespace neujson::internal;

TEST(clzll, normal) {
  EXPECT_EQ(clzll(1), 63U);
  EXPECT_EQ(clzll(2), 62U);
  EXPECT_EQ(clzll(12), 60U);
  EXPECT_EQ(clzll(0x0000000080000001UL), 32U);
  EXPECT_EQ(clzll(0x8000000000000001UL), 0U);
}
