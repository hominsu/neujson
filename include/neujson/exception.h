//
// Created by Homin Su on 2022/3/7.
//

#ifndef NEUJSON_NEUJSON_EXCEPTION_H_
#define NEUJSON_NEUJSON_EXCEPTION_H_

#include <exception>

#include "neujson.h"

namespace neujson {

#define ERR_TABLE(XX) \
  XX(OK, "ok")        \
  XX(ROOT_NOT_SINGULAR, "root not singular") \
  XX(BAD_VALUE, "bad value")                 \
  XX(EXPECT_VALUE, "expect value")           \
  XX(NUMBER_TOO_BIG, "number too big")       \
  XX(BAD_STRING_CHAR, "bad character")       \
  XX(BAD_STRING_ESCAPE, "bad escape")        \
  XX(BAD_UNICODE_HEX, "bad unicode hex")     \
  XX(BAD_UNICODE_SURROGATE, "bad unicode surrogate") \
  XX(MISS_QUOTATION_MARK, "miss quotation mark")     \
  XX(MISS_COMMA_OR_SQUARE_BRACKET, "miss comma or square bracket") \
  XX(MISS_KEY, "miss key")                   \
  XX(MISS_COLON, "miss colon")               \
  XX(MISS_COMMA_OR_CURLY_BRACKET, "miss comma or curly bracket")   \
  XX(USER_STOPPED, "user stopped parse")     \
  //

namespace error {
enum ParseError {
#define ERR_NO(_err, _str) PARSE_##_err,
  ERR_TABLE(ERR_NO)
#undef ERR_NO
};
} // namespace error

inline const char *parseErrorStr(error::ParseError _err) {
  const static char *err_str_table[] = {
#define ERR_STR(_err, _str) _str,
      ERR_TABLE(ERR_STR)
#undef ERR_STR
  };

  NEUJSON_ASSERT(_err >= 0 && _err < sizeof(err_str_table) / sizeof(err_str_table[0]));
  return err_str_table[_err];
}

class Exception : public ::std::exception {
 private:
  error::ParseError err_;

 public:
  explicit Exception(error::ParseError _err) : err_(_err) {}

  [[nodiscard]] const char *what() const noexcept override { return parseErrorStr(err_); }
  [[nodiscard]] error::ParseError err() const { return err_; }
};

#undef ERR_TABLE

} // namespace neujson

#endif //NEUJSON_NEUJSON_EXCEPTION_H_
