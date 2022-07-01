//
// Created by HominSu on 2022/6/30.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_INTERNAL_STRTOD_H_
#define NEUJSON_INCLUDE_NEUJSON_INTERNAL_STRTOD_H_

#include "diy_fp.h"
#include "ieee754.h"
#include "pow10.h"

namespace neujson::internal {

inline double FastPath(double _fraction, int _exponent) {
  if (_exponent < -308) { return 0.0; }
  else if (_exponent >= 0) { return _fraction * Pow10(_exponent); }
  else { return _fraction * Pow10(-_exponent); }
}

inline double StrTodNormalPrecision(double _d, int _p) {
  if (_p < -308) {
    _d = FastPath(_d, -308);
    _d = FastPath(_d, _p + 308);
  } else {
    _d = FastPath(_d, _p);
  }
  return _d;
}

} // namespace neujson::internal

#endif //NEUJSON_INCLUDE_NEUJSON_INTERNAL_STRTOD_H_
