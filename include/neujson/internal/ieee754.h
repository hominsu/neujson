//
// Created by HominSu on 2022/6/28.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_INTERNAL_IEEE754_H_
#define NEUJSON_INCLUDE_NEUJSON_INTERNAL_IEEE754_H_

#include "neujson/neujson.h"

#include <cstdint>

namespace neujson::internal {

class Double {
 private:
  union {
    double d_;
    uint64_t u_;
  };

 public:
  Double() = default;
  explicit Double(double _d) { d_ = _d; }   // NOLINT
  explicit Double(uint64_t _u) { u_ = _u; } // NOLINT

};

} // namespace neujson::internal

#endif //NEUJSON_INCLUDE_NEUJSON_INTERNAL_IEEE754_H_
