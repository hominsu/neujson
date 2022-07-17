//
// Created by Homin Su on 2022/7/16.
//

#include "unit_test.h"

#include "neujson/document.h"
#include "neujson/exception.h"
#include "neujson/string_write_stream.h"
#include "neujson/value.h"
#include "neujson/writer.h"

#include <string>
#include <string_view>

#define TEST_ROUNDTRIP_BOOL(_json)\
  do {\
    ::std::string_view ssi((_json));\
    neujson::Document doc;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, doc.parse(ssi));\
    EXPECT_EQ(neujson::NEU_BOOL, doc.GetType());\
    neujson::StringWriteStream oss;\
    neujson::Writer<neujson::StringWriteStream> writer(oss);\
    doc.WriteTo(writer);\
    EXPECT_STREQ((_json), oss.get().data());\
  } while (0)
  //

TEST(round_trip, bool) {
  TEST_ROUNDTRIP_BOOL("true");
  TEST_ROUNDTRIP_BOOL("false");
}

#define TEST_ROUNDTRIP_INT32(_json)\
  do {\
    ::std::string_view ssi((_json));\
    neujson::Document doc;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, doc.parse(ssi));\
    EXPECT_EQ(neujson::NEU_INT32, doc.GetType());\
    neujson::StringWriteStream oss;\
    neujson::Writer<neujson::StringWriteStream> writer(oss);\
    doc.WriteTo(writer);\
    EXPECT_STREQ((_json), ::std::string(oss.get()).c_str());\
  } while (0)
  //

TEST(round_trip, int32) {
  TEST_ROUNDTRIP_INT32("0");
  TEST_ROUNDTRIP_INT32("1234567890");
  TEST_ROUNDTRIP_INT32("-1234567890");
  TEST_ROUNDTRIP_INT32("2147483647");
  TEST_ROUNDTRIP_INT32("-2147483648");
}

#define TEST_ROUNDTRIP_INT64(_json)\
  do {\
    ::std::string_view ssi((_json));\
    neujson::Document doc;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, doc.parse(ssi));\
    EXPECT_TRUE(neujson::NEU_INT64 == doc.GetType() || neujson::NEU_INT32 == doc.GetType());\
    neujson::StringWriteStream oss;\
    neujson::Writer<neujson::StringWriteStream> writer(oss);\
    doc.WriteTo(writer);\
    EXPECT_STREQ((_json), ::std::string(oss.get()).c_str());\
  } while (0)
  //

TEST(round_trip, int64) {
  TEST_ROUNDTRIP_INT64("0");
  TEST_ROUNDTRIP_INT64("12345678901234");
  TEST_ROUNDTRIP_INT64("-12345678901234");
  TEST_ROUNDTRIP_INT64("9223372036854775807");
  TEST_ROUNDTRIP_INT64("-9223372036854775808");
}

#define TEST_ROUNDTRIP_DOUBLE(_json)\
  do {\
    ::std::string_view ssi((_json));\
    neujson::Document doc;\
    EXPECT_EQ(neujson::error::ParseError::PARSE_OK, doc.parse(ssi));\
    EXPECT_EQ(neujson::NEU_DOUBLE, doc.GetType());\
    neujson::StringWriteStream oss;\
    neujson::Writer<neujson::StringWriteStream> writer(oss);\
    doc.WriteTo(writer);\
    EXPECT_STREQ((_json), ::std::string(oss.get()).c_str());\
  } while (0)
  //

TEST(round_trip, double) {
  TEST_ROUNDTRIP_DOUBLE("1.5");
  TEST_ROUNDTRIP_DOUBLE("-1.5");
  TEST_ROUNDTRIP_DOUBLE("3.25");
  TEST_ROUNDTRIP_DOUBLE("1e+20");
  TEST_ROUNDTRIP_DOUBLE("1.234e+20");
  TEST_ROUNDTRIP_DOUBLE("1.234e-20");
}
