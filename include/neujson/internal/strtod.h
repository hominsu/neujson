//
// Created by Homing So on 2022/6/30.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_INTERNAL_STRTOD_H_
#define NEUJSON_INCLUDE_NEUJSON_INTERNAL_STRTOD_H_

#include "diy_fp.h"
#include "ieee754.h"
#include "pow10.h"

namespace neujson::internal {

inline double FastPath(const double fraction, const int exponent) {
  if (exponent < -308) {
    return 0.0;
  } else if (exponent >= 0) {
    return fraction * Pow10(exponent);
  } else {
    return fraction * Pow10(-exponent);
  }
}

inline double StrTodNormalPrecision(double d, int p) {
  if (p < -308) {
    d = FastPath(d, -308);
    d = FastPath(d, p + 308);
  } else {
    d = FastPath(d, p);
  }
  return d;
}

} // namespace neujson::internal

#endif // NEUJSON_INCLUDE_NEUJSON_INTERNAL_STRTOD_H_
