//
// Created by Homing So on 2022/7/5.
//

#include <cstdint>

#include <string>
#include <string_view>
#include <utility>

#include "neujson/document.h"
#include "neujson/exception.h"
#include "neujson/internal/ieee754.h"
#include "neujson/non_copyable.h"
#include "neujson/reader.h"
#include "neujson/string_read_stream.h"
#include "neujson/string_write_stream.h"
#include "neujson/value.h"
#include "neujson/writer.h"

#include "gtest/gtest.h"

#if defined(_MSC_VER) && !defined(__clang__)
NEUJSON_DIAG_PUSH
#elif defined(__GNUC__)
NEUJSON_DIAG_PUSH
NEUJSON_DIAG_OFF(effc++)
#endif

class TestHandler : neujson::NonCopyable {
  neujson::Value value_;
  neujson::Type value_type_ = neujson::NEU_NULL;

  neujson::Type test_error_type_ = neujson::NEU_BOOL;
  bool test_error_ = false;

public:
  bool Null() {
    addValue(neujson::Value(neujson::NEU_NULL));
    return true;
  }

  bool Bool(const bool b) {
    addValue(neujson::Value(b));
    return true;
  }

  bool Int32(const int32_t i32) {
    addValue(neujson::Value(i32));
    return true;
  }

  bool Int64(const int64_t i64) {
    addValue(neujson::Value(i64));
    return true;
  }

  bool Double(const neujson::internal::Double d) {
    addValue(neujson::Value(d));
    return true;
  }

  bool String(const std::string_view str) {
    addValue(neujson::Value(str));
    return true;
  }

  bool Key(const std::string_view str) {
    addValue(neujson::Value(str));
    return true;
  }

  bool StartObject() {
    addValue(neujson::Value(neujson::NEU_OBJECT));
    return true;
  }

  bool EndObject() {
    addValue(neujson::Value(neujson::NEU_OBJECT));
    return true;
  }

  bool StartArray() {
    addValue(neujson::Value(neujson::NEU_ARRAY));
    return true;
  }

  bool EndArray() {
    addValue(neujson::Value(neujson::NEU_ARRAY));
    return true;
  }

  [[nodiscard]] neujson::Type type() const { return value_type_; }
  [[nodiscard]] neujson::Value value() const { return value_; }

  void set_test_error(const bool test_error) { test_error_ = test_error; }
  void set_test_error_type(const neujson::Type type) {
    test_error_type_ = type;
  }

private:
  void addValue(neujson::Value &&value) {
    if (!test_error_) {
      value_type_ = value.GetType();
    }
    value_ = std::move(value);
  }
};

TEST(parse, null) {
  neujson::StringWriteStream write_stream;
  neujson::Writer writer(write_stream);

  writer.Null();

  neujson::StringReadStream read_stream(write_stream.get());
  TestHandler test_handler;

  EXPECT_EQ(neujson::error::ParseError::OK,
            neujson::Reader::Parse(read_stream, test_handler));
  EXPECT_EQ(neujson::NEU_NULL, test_handler.type());
}

#define TEST_BOOL(_expect, _json)                                              \
  do {                                                                         \
    std::string_view ss((_json));                                              \
    neujson::StringReadStream read_stream(ss);                                 \
    TestHandler test_handler;                                                  \
    EXPECT_EQ(neujson::error::ParseError::OK,                                  \
              neujson::Reader::Parse(read_stream, test_handler));              \
    EXPECT_EQ(neujson::NEU_BOOL, test_handler.type());                         \
    EXPECT_EQ((_expect), test_handler.value().GetBool());                      \
  } while (0) //

TEST(parse, bool) {
  TEST_BOOL(true, "true");
  TEST_BOOL(false, "false");
}

#define TEST_INT32(_expect, _json)                                             \
  do {                                                                         \
    std::string_view ss((_json));                                              \
    neujson::StringReadStream read_stream(ss);                                 \
    TestHandler test_handler;                                                  \
    EXPECT_EQ(neujson::error::ParseError::OK,                                  \
              neujson::Reader::Parse(read_stream, test_handler));              \
    EXPECT_EQ(neujson::NEU_INT32, test_handler.type());                        \
    EXPECT_EQ((_expect), test_handler.value().GetInt32());                     \
  } while (0) //

TEST(parse, int32) {
  TEST_INT32(0, "0");
  TEST_INT32(1234567890, "1234567890");
  TEST_INT32(-1234567890, "-1234567890");
  TEST_INT32(INT32_MAX, "2147483647");
  TEST_INT32(INT32_MIN, "-2147483648");
}

#define TEST_INT64(_expect, _json)                                             \
  do {                                                                         \
    std::string_view ss((_json));                                              \
    neujson::StringReadStream read_stream(ss);                                 \
    TestHandler test_handler;                                                  \
    EXPECT_EQ(neujson::error::ParseError::OK,                                  \
              neujson::Reader::Parse(read_stream, test_handler));              \
    EXPECT_TRUE(neujson::NEU_INT64 == test_handler.type() ||                   \
                neujson::NEU_INT32 == test_handler.type());                    \
    EXPECT_EQ((_expect), test_handler.value().GetInt64());                     \
  } while (0) //

TEST(parse, int64) {
  TEST_INT64(0, "0");
  TEST_INT64(12345678901234, "12345678901234");
  TEST_INT64(-12345678901234, "-12345678901234");
  TEST_INT64(INT64_MAX, "9223372036854775807");
  TEST_INT64(INT64_MIN, "-9223372036854775808");
}

#define TEST_DOUBLE(_expect, _json)                                            \
  do {                                                                         \
    std::string_view ss((_json));                                              \
    neujson::StringReadStream read_stream(ss);                                 \
    TestHandler test_handler;                                                  \
    EXPECT_EQ(neujson::error::ParseError::OK,                                  \
              neujson::Reader::Parse(read_stream, test_handler));              \
    EXPECT_EQ(neujson::NEU_DOUBLE, test_handler.type());                       \
    EXPECT_DOUBLE_EQ((_expect), test_handler.value().GetDouble());             \
  } while (0) //

TEST(parse, double) {
  TEST_DOUBLE(0.0, "0.0");
  TEST_DOUBLE(0.0, "-0.0");
  TEST_DOUBLE(1.0, "1.0");
  TEST_DOUBLE(-1.0, "-1.0");
  TEST_DOUBLE(1.5, "1.5");
  TEST_DOUBLE(-1.5, "-1.5");
  TEST_DOUBLE(3.1416, "3.1416");
  TEST_DOUBLE(1E10, "1E10");
  TEST_DOUBLE(1e10, "1e10");
  TEST_DOUBLE(1E+10, "1E+10");
  TEST_DOUBLE(1E-10, "1E-10");
  TEST_DOUBLE(-1E10, "-1E10");
  TEST_DOUBLE(-1e10, "-1e10");
  TEST_DOUBLE(-1E+10, "-1E+10");
  TEST_DOUBLE(-1E-10, "-1E-10");
  TEST_DOUBLE(1.234E+10, "1.234E+10");
  TEST_DOUBLE(1.234E-10, "1.234E-10");

  //  TEST_DOUBLE(0.0, "1e-10000"); /* must underflow */
  //  TEST_DOUBLE(1.0000000000000002, "1.0000000000000002"); /* the smallest
  //  number > 1 */ TEST_DOUBLE(4.9406564584124654e-324,
  //  "4.9406564584124654e-324"); /* minimum denormal */
  //  TEST_DOUBLE(-4.9406564584124654e-324, "-4.9406564584124654e-324");
  //  TEST_DOUBLE(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max
  //  subnormal double */ TEST_DOUBLE(-2.2250738585072009e-308,
  //  "-2.2250738585072009e-308"); TEST_DOUBLE(2.2250738585072014e-308,
  //  "2.2250738585072014e-308");  /* Min normal positive double */
  //  TEST_DOUBLE(-2.2250738585072014e-308, "-2.2250738585072014e-308");
  //  TEST_DOUBLE(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max
  //  double */ TEST_DOUBLE(-1.7976931348623157e+308,
  //  "-1.7976931348623157e+308");
}

#define TEST_STRING(_expect, _json)                                            \
  do {                                                                         \
    std::string_view ss((_json));                                              \
    neujson::StringReadStream read_stream(ss);                                 \
    TestHandler test_handler;                                                  \
    EXPECT_EQ(neujson::error::ParseError::OK,                                  \
              neujson::Reader::Parse(read_stream, test_handler));              \
    EXPECT_EQ(neujson::NEU_STRING, test_handler.type());                       \
    EXPECT_STREQ((_expect), test_handler.value().GetString().c_str());         \
  } while (0) //

TEST(parse, string) {
  TEST_STRING("", R"("")");
  TEST_STRING("Hello", R"("Hello")");
  TEST_STRING("Hello\nWorld", R"("Hello\nWorld")");
  TEST_STRING("\" \\ / \b \f \n \r \t", R"("\" \\ / \b \f \n \r \t")");
  TEST_STRING("Hello\0World", R"("Hello\u0000World")");
  TEST_STRING("\x24", R"("\u0024")");         /* Dollar sign U+0024 */
  TEST_STRING("\xC2\xA2", R"("\u00A2")");     /* Cents sign U+00A2 */
  TEST_STRING("\xE2\x82\xAC", R"("\u20AC")"); /* Euro sign U+20AC */
  TEST_STRING("\xF0\x9D\x84\x9E",
              R"("\uD834\uDD1E")"); /* G clef sign U+1D11E */
  TEST_STRING("\xF0\x9D\x84\x9E",
              R"("\ud834\udd1e")"); /* G clef sign U+1D11E */
}

TEST(parse, array) {
  {
    std::string_view ss("[ ]");
    neujson::StringReadStream read_stream(ss);
    TestHandler test_handler;
    EXPECT_EQ(neujson::error::ParseError::OK,
              neujson::Reader::Parse(read_stream, test_handler));
    EXPECT_EQ(neujson::NEU_ARRAY, test_handler.type());
    EXPECT_EQ(0UL, test_handler.value().GetArray()->size());
  }
  {
    std::string_view ss(R"([ null , false , true , 123 , "abc" ])");
    neujson::Document doc;
    EXPECT_EQ(neujson::error::ParseError::OK, doc.Parse(ss));
    EXPECT_EQ(neujson::NEU_ARRAY, doc.GetType());
    EXPECT_EQ(5UL, doc.GetArray()->size());
    EXPECT_EQ(neujson::Type::NEU_NULL, doc.GetArray()->at(0).GetType());
    EXPECT_EQ(neujson::Type::NEU_BOOL, doc.GetArray()->at(1).GetType());
    EXPECT_EQ(neujson::Type::NEU_BOOL, doc.GetArray()->at(2).GetType());
    EXPECT_EQ(neujson::Type::NEU_INT32, doc.GetArray()->at(3).GetType());
    EXPECT_EQ(neujson::Type::NEU_STRING, doc.GetArray()->at(4).GetType());
    EXPECT_EQ(false, doc.GetArray()->at(1).GetBool());
    EXPECT_EQ(true, doc.GetArray()->at(2).GetBool());
    EXPECT_EQ(123, doc.GetArray()->at(3).GetInt32());
    EXPECT_STREQ("abc", doc.GetArray()->at(4).GetString().c_str());
  }
  {
    std::string_view ss(R"([ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ])");
    neujson::Document doc;
    EXPECT_EQ(neujson::error::ParseError::OK, doc.Parse(ss));
    EXPECT_EQ(neujson::NEU_ARRAY, doc.GetType());
    EXPECT_EQ(4UL, doc.GetArray()->size());
    for (std::size_t i = 0; i < 4; i++) {
      auto a = doc.GetArray()->at(i);
      EXPECT_EQ(neujson::Type::NEU_ARRAY, a.GetType());
      EXPECT_EQ(i, a.GetArray()->size());
      for (std::size_t j = 0; j < i; j++) {
        auto e = a.GetArray()->at(j);
        EXPECT_EQ(neujson::Type::NEU_INT32, e.GetType());
        EXPECT_EQ(static_cast<int32_t>(j), e.GetInt32());
      }
    }
  }
}

TEST(parse, object) {
  {
    std::string_view ss(" { } ");
    neujson::StringReadStream read_stream(ss);
    TestHandler test_handler;
    EXPECT_EQ(neujson::error::ParseError::OK,
              neujson::Reader::Parse(read_stream, test_handler));
    EXPECT_EQ(neujson::NEU_OBJECT, test_handler.type());
    EXPECT_EQ(0UL, test_handler.value().GetObject()->size());
  }
  {
    std::string_view ss(R"(
{
    "n" : null ,
    "f" : false ,
    "t" : true ,
    "i" : 123 ,
    "s" : "abc",
    "a" : [ 1, 2, 3 ],
    "o" : { "1" : 1, "2" : 2, "3" : 3 }
}
)");
    neujson::Document doc;
    EXPECT_EQ(neujson::error::ParseError::OK, doc.Parse(ss));
    EXPECT_EQ(neujson::NEU_OBJECT, doc.GetType());
    EXPECT_EQ(7UL, doc.GetObject()->size());

    EXPECT_STREQ("n", doc.GetObject()->at(0).key_.GetString().c_str());
    EXPECT_EQ(neujson::NEU_NULL, doc.GetObject()->at(0).value_.GetType());

    EXPECT_STREQ("f", doc.GetObject()->at(1).key_.GetString().c_str());
    EXPECT_EQ(neujson::NEU_BOOL, doc.GetObject()->at(1).value_.GetType());
    EXPECT_EQ(false, doc.GetObject()->at(1).value_.GetBool());

    EXPECT_STREQ("t", doc.GetObject()->at(2).key_.GetString().c_str());
    EXPECT_EQ(neujson::NEU_BOOL, doc.GetObject()->at(2).value_.GetType());
    EXPECT_EQ(true, doc.GetObject()->at(2).value_.GetBool());

    EXPECT_STREQ("i", doc.GetObject()->at(3).key_.GetString().c_str());
    EXPECT_EQ(neujson::NEU_INT32, doc.GetObject()->at(3).value_.GetType());
    EXPECT_EQ(123, doc.GetObject()->at(3).value_.GetInt32());

    EXPECT_STREQ("s", doc.GetObject()->at(4).key_.GetString().c_str());
    EXPECT_EQ(neujson::NEU_STRING, doc.GetObject()->at(4).value_.GetType());
    EXPECT_STREQ("abc", doc.GetObject()->at(4).value_.GetString().c_str());

    EXPECT_STREQ("a", doc.GetObject()->at(5).key_.GetString().c_str());
    EXPECT_EQ(neujson::NEU_ARRAY, doc.GetObject()->at(5).value_.GetType());
    EXPECT_EQ(3UL, doc.GetObject()->at(5).value_.GetArray()->size());
    for (std::size_t i = 0; i < 3; i++) {
      auto e = doc.GetObject()->at(5).value_.GetArray();
      EXPECT_EQ(neujson::NEU_INT32, e->at(i).GetType());
      EXPECT_EQ(static_cast<int32_t>(i + 1), e->at(i).GetInt32());
    }

    EXPECT_STREQ("o", doc.GetObject()->at(6).key_.GetString().c_str());
    EXPECT_EQ(neujson::NEU_OBJECT, doc.GetObject()->at(6).value_.GetType());
    EXPECT_EQ(3UL, doc.GetObject()->at(6).value_.GetObject()->size());
    for (std::size_t i = 0; i < 3; i++) {
      auto object_key = doc.GetObject()->at(6).value_.GetObject()->at(i).key_;
      auto object_value =
          doc.GetObject()->at(6).value_.GetObject()->at(i).value_;
      char s[2]{};
      s[0] = static_cast<char>('1' + i);
      EXPECT_STREQ(s, object_key.GetString().c_str());
      EXPECT_EQ(1UL, object_key.GetString().size());
      EXPECT_EQ(neujson::NEU_INT32, object_value.GetType());
      EXPECT_EQ(static_cast<int32_t>(i + 1), object_value.GetInt32());
    }
  }
}

#define TEST_PARSE_ERROR(_error, _json)                                        \
  do {                                                                         \
    std::string_view ss((_json));                                              \
    neujson::StringReadStream read_stream(ss);                                 \
    TestHandler test_handler;                                                  \
    EXPECT_EQ((_error), neujson::Reader::Parse(read_stream, test_handler));    \
  } while (0) //

TEST(parse, expect_value) {
  TEST_PARSE_ERROR(neujson::error::EXPECT_VALUE, "");
  TEST_PARSE_ERROR(neujson::error::EXPECT_VALUE, " ");
}

TEST(parse, bad_value) {
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "nul");
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "?");

  // invalid number
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "+0");
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "+1");
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE,
                   ".123"); // at least one digit before '.'
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE,
                   "1."); // at least one digit after '.'
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "INF");
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "inf");
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "NAN");
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "nan");

  // invalid value in array
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, "[1,]");
  TEST_PARSE_ERROR(neujson::error::BAD_VALUE, R"(["a", nul])");
}

TEST(parse, root_not_singular) {
  TEST_PARSE_ERROR(neujson::error::ROOT_NOT_SINGULAR, "null x");

  // invalid number
  TEST_PARSE_ERROR(neujson::error::ROOT_NOT_SINGULAR,
                   "0123"); // after zero should be '.' , 'E' , 'e' or nothing
  TEST_PARSE_ERROR(neujson::error::ROOT_NOT_SINGULAR, "0x0");
  TEST_PARSE_ERROR(neujson::error::ROOT_NOT_SINGULAR, "0x123");
}

TEST(parse, number_too_big) {
  TEST_PARSE_ERROR(neujson::error::NUMBER_TOO_BIG, "1e309");
  TEST_PARSE_ERROR(neujson::error::NUMBER_TOO_BIG, "-1e309");
}

TEST(parse, miss_quotation_mark) {
  TEST_PARSE_ERROR(neujson::error::MISS_QUOTATION_MARK, R"(")");
  TEST_PARSE_ERROR(neujson::error::MISS_QUOTATION_MARK, R"("abc)");
}

TEST(parse, bad_string_escape) {
  TEST_PARSE_ERROR(neujson::error::BAD_STRING_ESCAPE, R"("\v")");
  TEST_PARSE_ERROR(neujson::error::BAD_STRING_ESCAPE, R"("\'")");
  TEST_PARSE_ERROR(neujson::error::BAD_STRING_ESCAPE, R"("\0")");
  TEST_PARSE_ERROR(neujson::error::BAD_STRING_ESCAPE, R"("\x12")");
}

TEST(parse, bad_string_char) {
  TEST_PARSE_ERROR(neujson::error::BAD_STRING_CHAR, "\"\x01\"");
  TEST_PARSE_ERROR(neujson::error::BAD_STRING_CHAR, "\"\x1F\"");
}

TEST(parse, bad_unicode_hex) {
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u0")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u01")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u012")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u/000")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\uG000")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u0/00")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u0G00")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u00/0")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u00G0")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u000/")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u000G")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_HEX, R"("\u 123")");
}

TEST(parse, bad_unicode_surrogate) {
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_SURROGATE, R"("\uD800")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_SURROGATE, R"("\uDBFF")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_SURROGATE, R"("\uD800\\")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_SURROGATE, R"("\uD800\uDBFF")");
  TEST_PARSE_ERROR(neujson::error::BAD_UNICODE_SURROGATE, R"("\uD800\uE000")");
}

TEST(parse, miss_comma_or_square_bracket) {
  TEST_PARSE_ERROR(neujson::error::MISS_COMMA_OR_SQUARE_BRACKET, "[1");
  TEST_PARSE_ERROR(neujson::error::MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
  TEST_PARSE_ERROR(neujson::error::MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
  TEST_PARSE_ERROR(neujson::error::MISS_COMMA_OR_SQUARE_BRACKET, "[[]");
}

TEST(parse, miss_key) {
  TEST_PARSE_ERROR(neujson::error::MISS_KEY, R"({:1,)");
  TEST_PARSE_ERROR(neujson::error::MISS_KEY, R"({1:1,)");
  TEST_PARSE_ERROR(neujson::error::MISS_KEY, R"({true:1,)");
  TEST_PARSE_ERROR(neujson::error::MISS_KEY, R"({false:1,)");
  TEST_PARSE_ERROR(neujson::error::MISS_KEY, R"({null:1,)");
  TEST_PARSE_ERROR(neujson::error::MISS_KEY, R"({[]:1,)");
  TEST_PARSE_ERROR(neujson::error::MISS_KEY, R"({{}:1,)");
  TEST_PARSE_ERROR(neujson::error::MISS_KEY, R"({"a":1,)");
}

TEST(parse, miss_colon) {
  TEST_PARSE_ERROR(neujson::error::MISS_COLON, R"({"a"})");
  TEST_PARSE_ERROR(neujson::error::MISS_COLON, R"({"a","b"})");
}

TEST(parse, miss_comma_or_curly_bracket) {
  TEST_PARSE_ERROR(neujson::error::MISS_COMMA_OR_CURLY_BRACKET, R"({"a":1)");
  TEST_PARSE_ERROR(neujson::error::MISS_COMMA_OR_CURLY_BRACKET, R"({"a":1])");
  TEST_PARSE_ERROR(neujson::error::MISS_COMMA_OR_CURLY_BRACKET,
                   R"({"a":1 "b")");
  TEST_PARSE_ERROR(neujson::error::MISS_COMMA_OR_CURLY_BRACKET, R"({"a":{})");
}

#if defined(__GNUC__) || (defined(_MSC_VER) && !defined(__clang__))
NEUJSON_DIAG_POP
#endif