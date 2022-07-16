//
// Created by Homin Su on 2022/7/16.
//

#include "unit_test.h"

#include "neujson/document.h"
#include "neujson/exception.h"
#include "neujson/string_write_stream.h"
#include "neujson/value.h"
#include "neujson/writer.h"

#include <cstdint>

#include <string>
#include <string_view>

#define TEST_BOOL(_expect, _json)\
  do {\
    ::std::string_view ssi((_json));\
    neujson::Document doc;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, doc.parse(ssi));\
    EXPECT_EQ(neujson::NEU_BOOL, doc.GetType());\
    EXPECT_EQ((_expect), doc.GetBool());\
    neujson::StringWriteStream oss;\
    neujson::Writer<neujson::StringWriteStream> writer(oss);\
    doc.WriteTo(writer);\
    EXPECT_STREQ((_json), oss.get().data());\
  } while (0)

TEST(round_trip, bool) {
  TEST_BOOL(true, "true");
  TEST_BOOL(false, "false");
}

#define TEST_INT32(_expect, _json)\
  do {\
    ::std::string_view ssi((_json));\
    neujson::Document doc;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, doc.parse(ssi));\
    EXPECT_EQ(neujson::NEU_INT32, doc.GetType());\
    EXPECT_EQ((_expect), doc.GetInt32());\
    neujson::StringWriteStream oss;\
    neujson::Writer<neujson::StringWriteStream> writer(oss);\
    doc.WriteTo(writer);\
    EXPECT_STREQ((_json), ::std::string(oss.get()).c_str());\
  } while (0)

TEST(round_trip, int32) {
  TEST_INT32(0, "0");
  TEST_INT32(1234567890, "1234567890");
  TEST_INT32(-1234567890, "-1234567890");
  TEST_INT32(INT32_MAX, "2147483647");
  TEST_INT32(INT32_MIN, "-2147483648");
}

#define TEST_INT64(_expect, _json)\
  do {\
    ::std::string_view ssi((_json));\
    neujson::Document doc;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, doc.parse(ssi));\
    EXPECT_TRUE(neujson::NEU_INT64 == doc.GetType() || neujson::NEU_INT32 == doc.GetType());\
    EXPECT_EQ((_expect), doc.GetInt64());\
    neujson::StringWriteStream oss;\
    neujson::Writer<neujson::StringWriteStream> writer(oss);\
    doc.WriteTo(writer);\
    EXPECT_STREQ((_json), ::std::string(oss.get()).c_str());\
  } while (0)

TEST(round_trip, int64) {
  TEST_INT64(0, "0");
  TEST_INT64(12345678901234, "12345678901234");
  TEST_INT64(-12345678901234, "-12345678901234");
  TEST_INT64(INT64_MAX, "9223372036854775807");
  TEST_INT64(INT64_MIN, "-9223372036854775808");
}

#define TEST_DOUBLE(_expect, _json)\
  do {\
    ::std::string_view ssi((_json));\
    neujson::Document doc;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, doc.parse(ssi));\
    EXPECT_EQ(neujson::NEU_DOUBLE, doc.GetType());\
    EXPECT_DOUBLE_EQ((_expect), doc.GetDouble());\
    neujson::StringWriteStream oss;\
    neujson::Writer<neujson::StringWriteStream> writer(oss);\
    doc.WriteTo(writer);\
    EXPECT_STREQ((_json), ::std::string(oss.get()).c_str());\
  } while (0)

TEST(round_trip, double) {
  TEST_DOUBLE(1.5, "1.5");
  TEST_DOUBLE(-1.5, "-1.5");
  TEST_DOUBLE(3.25, "3.25");
  TEST_DOUBLE(1E+20, "1e+20");
  TEST_DOUBLE(1.234E+20, "1.234e+20");
  TEST_DOUBLE(1.234E-20, "1.234e-20");
}
