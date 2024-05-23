//
// Created by Homing So on 2022/3/7.
//

#ifndef NEUJSON_NEUJSON_EXCEPTION_H_
#define NEUJSON_NEUJSON_EXCEPTION_H_

#include <exception>

#include "neujson.h"

namespace neujson {

#define ERR_TABLE(ERR)                                                         \
  ERR(OK, "ok")                                                                \
  ERR(ROOT_NOT_SINGULAR, "root not singular")                                  \
  ERR(BAD_VALUE, "bad value")                                                  \
  ERR(EXPECT_VALUE, "expect value")                                            \
  ERR(NUMBER_TOO_BIG, "number too big")                                        \
  ERR(BAD_STRING_CHAR, "bad character")                                        \
  ERR(BAD_STRING_ESCAPE, "bad escape")                                         \
  ERR(BAD_UNICODE_HEX, "bad unicode hex")                                      \
  ERR(BAD_UNICODE_SURROGATE, "bad unicode surrogate")                          \
  ERR(MISS_QUOTATION_MARK, "miss quotation mark")                              \
  ERR(MISS_COMMA_OR_SQUARE_BRACKET, "miss comma or square bracket")            \
  ERR(MISS_KEY, "miss key")                                                    \
  ERR(MISS_COLON, "miss colon")                                                \
  ERR(MISS_COMMA_OR_CURLY_BRACKET, "miss comma or curly bracket")              \
  ERR(USER_STOPPED, "user stopped parse")                                      \
  //

namespace error {
enum ParseError {
#define ERR_NO(_err, _str) _err,
  ERR_TABLE(ERR_NO)
#undef ERR_NO
};
} // namespace error

inline const char *ParseErrorStr(const error::ParseError err) {
  const static char *err_str_table[] = {
#define ERR_STR(_err, _str) _str,
      ERR_TABLE(ERR_STR)
#undef ERR_STR
  };

  NEUJSON_ASSERT(err >= 0 &&
                 err < sizeof(err_str_table) / sizeof(err_str_table[0]));
  return err_str_table[err];
}

#undef ERR_TABLE

class Exception : public std::exception {
  error::ParseError err_;

public:
  explicit Exception(const error::ParseError err) : err_(err) {}

  [[nodiscard]] const char *what() const noexcept override {
    return ParseErrorStr(err_);
  }
  [[nodiscard]] error::ParseError err() const { return err_; }
};

} // namespace neujson

#endif // NEUJSON_NEUJSON_EXCEPTION_H_
