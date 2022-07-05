//
// Created by Homin Su on 2022/7/5.
//

#include "unit_test.h"

#include "neujson/exception.h"
#include "neujson/internal/ieee754.h"
#include "neujson/noncopyable.h"
#include "neujson/reader.h"
#include "neujson/string_read_stream.h"
#include "neujson/string_write_stream.h"
#include "neujson/value.h"
#include "neujson/writer.h"

#include <cstdint>

#include <string_view>
#include <utility>

class TestHandler : neujson::noncopyable {
 private:
  neujson::Value value_;
  neujson::Type value_type_ = neujson::NEU_NULL;

 public:
  //@formatter:off
  bool Null() { addValue(neujson::Value(neujson::NEU_NULL)); return true; }
  bool Bool(bool _b) { addValue(neujson::Value(_b)); return true; }
  bool Int32(int32_t _i32) { addValue(neujson::Value(_i32)); return true; }
  bool Int64(int64_t _i64) { addValue(neujson::Value(_i64)); return true; }
  bool Double(neujson::internal::Double _d) { addValue(neujson::Value(_d)); return true; }
  bool String(::std::string_view _str) { addValue(neujson::Value(_str)); return true; }
  bool Key(::std::string_view _str) { addValue(neujson::Value(_str)); return true; }
  bool StartObject() { addValue(neujson::Value(neujson::NEU_OBJECT)); return true; }
  bool EndObject() { addValue(neujson::Value(neujson::NEU_OBJECT)); return true; }
  bool StartArray() { addValue(neujson::Value(neujson::NEU_ARRAY)); return true; }
  bool EndArray() { addValue(neujson::Value(neujson::NEU_ARRAY)); return true; }
  //@formatter:on

  [[nodiscard]] neujson::Type type() const { return value_type_; }
  [[nodiscard]] neujson::Value value() const { return value_; }

 private:
  void addValue(neujson::Value &&_value) {
    value_type_ = _value.GetType();
    value_ = ::std::move(_value);
  }
};

TEST(parse, null) {
  neujson::StringWriteStream write_stream;
  neujson::Writer<neujson::StringWriteStream> writer(write_stream);

  writer.Null();

  neujson::StringReadStream read_stream(write_stream.get());
  TestHandler test_handler;

  EXPECT_EQ(neujson::error::ParseError::PARSE_OK, neujson::Reader::parse(read_stream, test_handler));
  EXPECT_EQ(neujson::NEU_NULL, test_handler.type());
}

TEST(parse, true) {
  neujson::StringWriteStream write_stream;
  neujson::Writer<neujson::StringWriteStream> writer(write_stream);

  writer.Bool(true);

  neujson::StringReadStream read_stream(write_stream.get());
  TestHandler test_handler;

  EXPECT_EQ(neujson::error::ParseError::PARSE_OK, neujson::Reader::parse(read_stream, test_handler));
  EXPECT_EQ(neujson::NEU_BOOL, test_handler.type());
  EXPECT_EQ(true, test_handler.value().GetBool());
}

TEST(parse, false) {
  neujson::StringWriteStream write_stream;
  neujson::Writer<neujson::StringWriteStream> writer(write_stream);

  writer.Bool(false);

  neujson::StringReadStream read_stream(write_stream.get());
  TestHandler test_handler;

  EXPECT_EQ(neujson::error::ParseError::PARSE_OK, neujson::Reader::parse(read_stream, test_handler));
  EXPECT_EQ(neujson::NEU_BOOL, test_handler.type());
  EXPECT_EQ(false, test_handler.value().GetBool());
}

#define TEST_INT32(_expect, _json)\
  do {\
    neujson::StringWriteStream write_stream;\
    neujson::Writer<neujson::StringWriteStream> writer(write_stream);\
    writer.Int32((_expect));\
    neujson::StringReadStream read_stream(write_stream.get());\
    TestHandler test_handler;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, neujson::Reader::parse(read_stream, test_handler));\
    EXPECT_EQ(neujson::NEU_INT32, test_handler.type());\
    EXPECT_EQ((_expect), test_handler.value().GetInt32());\
  } while (0)

TEST(parse, int32) {
  TEST_INT32(0, "0");
  TEST_INT32(1234567890, "1234567890");
  TEST_INT32(-1234567890, "-1234567890");
  TEST_INT32(INT32_MAX, "2147483647");
  TEST_INT32(INT32_MIN, "-2147483648");
}

#define TEST_INT64(_expect, _json)\
  do {\
    neujson::StringWriteStream write_stream;\
    neujson::Writer<neujson::StringWriteStream> writer(write_stream);\
    writer.Int64((_expect));\
    neujson::StringReadStream read_stream(write_stream.get());\
    TestHandler test_handler;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, neujson::Reader::parse(read_stream, test_handler));\
    EXPECT_TRUE(neujson::NEU_INT64 == test_handler.type() || neujson::NEU_INT32 == test_handler.type());\
    EXPECT_EQ((_expect), test_handler.value().GetInt64());\
  } while (0)

TEST(parse, int64) {
  TEST_INT64(0, "0");
  TEST_INT64(12345678901234, "12345678901234");
  TEST_INT64(-12345678901234, "-12345678901234");
  TEST_INT64(INT64_MAX, "9223372036854775807");
  TEST_INT64(INT64_MIN, "-9223372036854775808");
}