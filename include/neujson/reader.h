//
// Created by Homing So on 2022/3/7.
//

#ifndef NEUJSON_NEUJSON_READER_H_
#define NEUJSON_NEUJSON_READER_H_

#include <cmath>
#include <cstdint>

#include <exception>

#include "exception.h"
#include "internal/cllzl.h"
#include "non_copyable.h"
#include "value.h"

#if defined(NEUJSON_SSE42)
#include <nmmintrin.h>
#elif defined(NEUJSON_SSE2)
#include <emmintrin.h>
#elif defined(NEUJSON_NEON)
#include <arm_neon.h>
#endif

#if defined(NEUJSON_SSE42)
#undef NEUJSON_SSE42
#define NEUJSON_SSE42_OFF
#elif defined(NEUJSON_SSE2)
#undef NEUJSON_SSE2
#define NEUJSON_SSE2_OFF
#elif defined(NEUJSON_NEON)
#undef NEUJSON_NEON
#define NEUJSON_NEON_OFF
#endif

namespace neujson {

namespace required {

namespace read_stream {
namespace details {

template <typename ReadStream>
concept HasHasNext = requires(ReadStream rs) {
  { rs.hasNext() } -> std::same_as<bool>;
};

template <typename ReadStream>
concept HasPeek = requires(ReadStream rs) {
  { rs.peek() } -> std::same_as<char>;
};

template <typename ReadStream>
concept HasNext = requires(ReadStream rs) {
  { rs.next() } -> std::same_as<char>;
};

template <typename ReadStream>
concept HasAssertNext = requires(ReadStream rs, char ch) {
  { rs.assertNext(ch) } -> std::same_as<void>;
};

} // namespace details

template <typename T>
concept HasAllRequiredFunctions =
    details::HasHasNext<T> && details::HasPeek<T> && details::HasNext<T> &&
    details::HasAssertNext<T>;

} // namespace read_stream

namespace handler {
namespace details {

template <typename Handler>
concept HasNull = requires(Handler handler) {
  { handler.Null() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasBool = requires(Handler handler, bool b) {
  { handler.Bool(b) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasInt32 = requires(Handler handler, int32_t i32) {
  { handler.Int32(i32) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasInt64 = requires(Handler handler, int64_t i64) {
  { handler.Int64(i64) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasDouble = requires(Handler handler, internal::Double d) {
  { handler.Double(d) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasString = requires(Handler handler, std::string_view sv) {
  { handler.String(sv) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasKey = requires(Handler handler, std::string_view sv) {
  { handler.Key(sv) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasStartObject = requires(Handler handler) {
  { handler.StartObject() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasEndObject = requires(Handler handler) {
  { handler.EndObject() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasStartArray = requires(Handler handler) {
  { handler.StartArray() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasEndArray = requires(Handler handler) {
  { handler.EndArray() } -> std::same_as<bool>;
};

} // namespace details

template <typename T>
concept HasAllRequiredFunctions =
    details::HasNull<T> && details::HasBool<T> && details::HasInt32<T> &&
    details::HasInt64<T> && details::HasDouble<T> && details::HasString<T> &&
    details::HasKey<T> && details::HasStartObject<T> &&
    details::HasEndObject<T> && details::HasStartArray<T> &&
    details::HasEndArray<T>;
} // namespace handler

} // namespace required

class Reader : NonCopyable {
public:
  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static error::ParseError Parse(ReadStream &rs, Handler &handler);

private:
  template <required::read_stream::HasAllRequiredFunctions ReadStream>
  static unsigned ParseHex4(ReadStream &rs);

#if defined(NEUJSON_SSE42)
  template <required::read_stream::HasAllRequiredFunctions ReadStream>
  static void ParseWhitespaceSSE42(ReadStream &rs);
#elif defined(NEUJSON_SSE2)
  template <required::read_stream::HasAllRequiredFunctions ReadStream>
  static void ParseWhitespaceSSE2(ReadStream &rs);
#elif defined(NEUJSON_NEON)
  template <required::read_stream::HasAllRequiredFunctions ReadStream>
  static void ParseWhitespaceNEON(ReadStream &rs);
#else
  template <required::read_stream::HasAllRequiredFunctions ReadStream>
  static void ParseWhitespaceBasic(ReadStream &rs);
#endif

  template <required::read_stream::HasAllRequiredFunctions ReadStream>
  static void ParseWhitespace(ReadStream &rs);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseLiteral(ReadStream &rs, Handler &handler,
                           const char *literal, Type type);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseNumber(ReadStream &rs, Handler &handler);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseString(ReadStream &rs, Handler &handler, bool is_key);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseArray(ReadStream &rs, Handler &handler);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseObject(ReadStream &rs, Handler &handler);

  template <required::read_stream::HasAllRequiredFunctions ReadStream,
            required::handler::HasAllRequiredFunctions Handler>
  static void ParseValue(ReadStream &rs, Handler &handler);

  static bool IsDigit(const char ch) { return ch >= '0' && ch <= '9'; }
  static bool IsDigit1To9(const char ch) { return ch >= '1' && ch <= '9'; }
  static void EncodeUtf8(std::string &buffer, unsigned int u);
};

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
error::ParseError Reader::Parse(ReadStream &rs, Handler &handler) {
  try {
    ParseWhitespace(rs);
    ParseValue(rs, handler);
    ParseWhitespace(rs);
    if (rs.hasNext()) {
      throw Exception(error::ROOT_NOT_SINGULAR);
    }
    return error::OK;
  } catch (Exception &e) {
    return e.err();
  }
}

template <required::read_stream::HasAllRequiredFunctions ReadStream>
unsigned Reader::ParseHex4(ReadStream &rs) {
  unsigned int u = 0;
  for (int i = 0; i < 4; i++) {
    u <<= 4;
    if (const char ch = rs.next(); ch >= '0' && ch <= '9') {
      u |= static_cast<unsigned int>(ch - '0');
    } else if (ch >= 'a' && ch <= 'f') {
      u |= static_cast<unsigned int>(ch - 'a' + 10);
    } else if (ch >= 'A' && ch <= 'F') {
      u |= static_cast<unsigned int>(ch - 'A' + 10);
    } else {
      throw Exception(error::BAD_UNICODE_HEX);
    }
  }
  return u;
}

#if defined(NEUJSON_SSE42)
/**
 * @brief Skip whitespace with SSE 4.2 pcmpistrm instruction, testing 16 8-byte
 * characters at once.
 * @tparam ReadStream
 * @param rs
 */
template <required::read_stream::HasAllRequiredFunctions ReadStream>
void Reader::ParseWhitespaceSSE42(ReadStream &rs) {
  char ch = rs.peek();
  if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
    rs.next();
  } else {
    return;
  }

  // 16-byte align to the next boundary
  const char *p = rs.getAddr();
  const char *nextAligned = reinterpret_cast<const char *>(
      (reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
  while (p != nextAligned) {
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') {
      ++p;
      rs.next();
    } else {
      return;
    }
  }

  // The rest of string using SIMD
  static const char whitespace[16] = " \n\r\t";
  const __m128i w =
      _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespace[0]));

  for (;; p += 16, rs.next(16)) {
    const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
    const int r =
        _mm_cmpistri(w, s,
                     _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
                         _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
    // some characters are non-whitespace
    if (r != 16) {
      rs.next(r);
      return;
    }
  }
}
#elif defined(NEUJSON_SSE2)
/**
 * @brief Skip whitespace with SSE2 instructions, testing 16 8-byte characters
 * at once.
 * @tparam ReadStream
 * @param rs
 */
template <required::read_stream::HasAllRequiredFunctions ReadStream>
void Reader::ParseWhitespaceSSE2(ReadStream &rs) {
  // Fast return for single non-whitespace
  char ch = rs.peek();
  if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
    rs.next();
  } else {
    return;
  }

  // 16-byte align to the next boundary
  const char *p = rs.getAddr();
  const char *nextAligned = reinterpret_cast<const char *>(
      (reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
  while (p != nextAligned) {
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') {
      ++p;
      rs.next();
    } else {
      return;
    }
  }

  // The rest of string
#define C16(c) {c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c}
  static const char whitespaces[4][16] = {C16(' '), C16('\n'), C16('\r'),
                                          C16('\t')};
#undef C16

  const __m128i w0 =
      _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[0][0]));
  const __m128i w1 =
      _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[1][0]));
  const __m128i w2 =
      _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[2][0]));
  const __m128i w3 =
      _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[3][0]));

  for (;; p += 16, rs.next(16)) {
    const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
    __m128i x = _mm_cmpeq_epi8(s, w0);
    x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w1));
    x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w2));
    x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w3));
    auto r = static_cast<unsigned short>(~_mm_movemask_epi8(x));
    if (r != 0) { // some characters may be non-whitespace
#ifdef _MSC_VER // Find the index of first non-whitespace
      unsigned long offset;
      _BitScanForward(&offset, r);
      rs.next(offset);
      return;
#else
      rs.next(__builtin_ffs(r) - 1);
      return;
#endif
    }
  }
}
#elif defined(NEUJSON_NEON)
/**
 * @brief Skip whitespace with ARM Neon instructions, testing 16 8-byte
 * characters at once.
 * @tparam ReadStream
 * @param rs
 */
template <required::read_stream::HasAllRequiredFunctions ReadStream>
void Reader::ParseWhitespaceNEON(ReadStream &rs) {
  char ch = rs.peek();
  if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
    rs.next();
  } else {
    return;
  }

  // 16-byte align to the next boundary
  const char *p = rs.getAddr();
  const char *nextAligned = reinterpret_cast<const char *>(
      (reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
  while (p != nextAligned) {
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') {
      ++p;
      rs.next();
    } else {
      return;
    }
  }

  const uint8x16_t w0 = vmovq_n_u8(' ');
  const uint8x16_t w1 = vmovq_n_u8('\n');
  const uint8x16_t w2 = vmovq_n_u8('\r');
  const uint8x16_t w3 = vmovq_n_u8('\t');

  for (;; p += 16, rs.next(16)) {
    const uint8x16_t s = vld1q_u8(reinterpret_cast<const uint8_t *>(p));
    uint8x16_t x = vceqq_u8(s, w0);
    x = vorrq_u8(x, vceqq_u8(s, w1));
    x = vorrq_u8(x, vceqq_u8(s, w2));
    x = vorrq_u8(x, vceqq_u8(s, w3));

    x = vmvnq_u8(x);                                            // Negate
    x = vrev64q_u8(x);                                          // Rev in 64
    uint64_t low = vgetq_lane_u64(vreinterpretq_u64_u8(x), 0);  // extract
    uint64_t high = vgetq_lane_u64(vreinterpretq_u64_u8(x), 1); // extract

    if (low == 0) {
      if (high != 0) {
        uint32_t lz = internal::clzll(high);
        rs.next(8 + (lz >> 3));
        return;
      }
    } else {
      uint32_t lz = internal::clzll(low);
      rs.next(lz >> 3);
      return;
    }
  }
}
#else
template <required::read_stream::HasAllRequiredFunctions ReadStream>
void Reader::ParseWhitespaceBasic(ReadStream &rs) {
  while (rs.hasNext()) {
    if (const char ch = rs.peek();
        ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
      rs.next();
    } else {
      break;
    }
  }
}
#endif

template <required::read_stream::HasAllRequiredFunctions ReadStream>
void Reader::ParseWhitespace(ReadStream &rs) {
#if defined(NEUJSON_SSE42)
  return ParseWhitespaceSSE42(rs);
#elif defined(NEUJSON_SSE2)
  return ParseWhitespaceSSE2(rs);
#elif defined(NEUJSON_NEON)
  return ParseWhitespaceNEON(rs);
#else
  return ParseWhitespaceBasic(rs);
#endif
}

#define CALL(_expr)                                                            \
  if (!(_expr))                                                                \
  throw Exception(error::USER_STOPPED)

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseLiteral(ReadStream &rs, Handler &handler, const char *literal,
                          const Type type) {
  const char c = *literal;

  rs.assertNext(*literal++);
  for (; *literal != '\0'; literal++, rs.next()) {
    if (*literal != rs.peek()) {
      throw Exception(error::BAD_VALUE);
    }
  }

  switch (type) {
  case NEU_NULL:
    CALL(handler.Null());
    return;
  case NEU_BOOL:
    CALL(handler.Bool(c == 't'));
    return;
  case NEU_DOUBLE:
    CALL(handler.Double(
        internal::Double(c == 'N' ? std::numeric_limits<double>::quiet_NaN()
                                  : std::numeric_limits<double>::infinity())));
    return;
  default:
    NEUJSON_ASSERT(false && "bad type");
  }
}

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseNumber(ReadStream &rs, Handler &handler) {
  // parse 'NaN' (Not a Number) && 'Infinity'
  if (rs.peek() == 'N') {
    ParseLiteral(rs, handler, "NaN", NEU_DOUBLE);
    return;
  }
  if (rs.peek() == 'I') {
    ParseLiteral(rs, handler, "Infinity", NEU_DOUBLE);
    return;
  }

  std::string buffer;

  if (rs.peek() == '-') {
    buffer.push_back(rs.next());
  }
  if (rs.peek() == '0') {
    buffer.push_back(rs.next());
  } else {
    if (!IsDigit1To9(rs.peek())) {
      throw Exception(error::BAD_VALUE);
    }
    for (buffer.push_back(rs.next()); IsDigit(rs.peek());
         buffer.push_back(rs.next()))
      ;
  }

  auto expectType = NEU_NULL;

  if (rs.peek() == '.') {
    expectType = NEU_DOUBLE;
    buffer.push_back(rs.next());
    if (!IsDigit(rs.peek())) {
      throw Exception(error::BAD_VALUE);
    }
    for (buffer.push_back(rs.next()); IsDigit(rs.peek());
         buffer.push_back(rs.next()))
      ;
  }

  if (rs.peek() == 'e' || rs.peek() == 'E') {
    expectType = NEU_DOUBLE;
    buffer.push_back(rs.next());
    if (rs.peek() == '+' || rs.peek() == '-') {
      buffer.push_back(rs.next());
    }
    if (!IsDigit(rs.peek())) {
      throw Exception(error::BAD_VALUE);
    }
    for (buffer.push_back(rs.next()); IsDigit(rs.peek());
         buffer.push_back(rs.next()))
      ;
  }

  if (buffer.empty()) {
    throw Exception(error::BAD_VALUE);
  }

  try {
    std::size_t idx;
    if (expectType == NEU_DOUBLE) {
      double d;
#if defined(__clang__) || defined(_MSC_VER)
      d = std::stod(buffer, &idx);
#elif defined(__GNUC__)
      d = __gnu_cxx::__stoa(&std::strtod, "stod", buffer.data(), &idx);
#else
#error "complier no support"
#endif
      NEUJSON_ASSERT(buffer.size() == idx);
      CALL(handler.Double(internal::Double(d)));
    } else {
      int64_t i64;
#if defined(__clang__) || defined(_MSC_VER)
      i64 = std::stoll(buffer, &idx, 10);
#elif defined(__GNUC__)
      i64 = __gnu_cxx::__stoa(&std::strtoll, "stoll", buffer.data(), &idx, 10);
#else
#error "complier no support"
#endif
      if (i64 <= std::numeric_limits<int32_t>::max() &&
          i64 >= std::numeric_limits<int32_t>::min()) {
        CALL(handler.Int32(static_cast<int32_t>(i64)));
      } else {
        CALL(handler.Int64(i64));
      }
    }
  } catch (...) {
    throw Exception(error::NUMBER_TOO_BIG);
  }
}

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseString(ReadStream &rs, Handler &handler, const bool is_key) {
  rs.assertNext('"');
  std::string buffer;
  while (rs.hasNext()) {
    switch (char ch = rs.next()) {
    case '"':
      if (is_key) {
        CALL(handler.Key(std::move(buffer)));
      } else {
        CALL(handler.String(std::move(buffer)));
      }
      return;
#if defined(__clang__) || defined(__GNUC__)
    case '\x01' ... '\x1f':
      throw Exception(error::BAD_STRING_CHAR);
#endif
    case '\\':
      switch (rs.next()) {
      case '"':
        buffer.push_back('"');
        break;
      case '\\':
        buffer.push_back('\\');
        break;
      case '/':
        buffer.push_back('/');
        break;
      case 'b':
        buffer.push_back('\b');
        break;
      case 'f':
        buffer.push_back('\f');
        break;
      case 'n':
        buffer.push_back('\n');
        break;
      case 'r':
        buffer.push_back('\r');
        break;
      case 't':
        buffer.push_back('\t');
        break;
      case 'u': {
        // unicode stuff from Milo's tutorial
        unsigned u = ParseHex4(rs);
        if (u >= 0xD800 && u <= 0xDBFF) {
          if (rs.next() != '\\') {
            throw Exception(error::BAD_UNICODE_SURROGATE);
          }
          if (rs.next() != 'u') {
            throw Exception(error::BAD_UNICODE_SURROGATE);
          }
          if (const unsigned u2 = ParseHex4(rs); u2 >= 0xDC00 && u2 <= 0xDFFF) {
            u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
          } else {
            throw Exception(error::BAD_UNICODE_SURROGATE);
          }
        }
        EncodeUtf8(buffer, u);
        break;
      }
      default:
        throw Exception(error::BAD_STRING_ESCAPE);
      }
      break;
    default:
#if defined(_MSC_VER)
      if (static_cast<unsigned char>(ch) < 0x20) {
        throw Exception(error::BAD_STRING_CHAR);
      }
#endif
      buffer.push_back(ch);
    }
  }
  throw Exception(error::MISS_QUOTATION_MARK);
}

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseArray(ReadStream &rs, Handler &handler) {
  CALL(handler.StartArray());

  rs.assertNext('[');
  ParseWhitespace(rs);
  if (rs.peek() == ']') {
    rs.next();
    CALL(handler.EndArray());
    return;
  }

  while (true) {
    ParseValue(rs, handler);
    ParseWhitespace(rs);
    switch (rs.next()) {
    case ',':
      ParseWhitespace(rs);
      break;
    case ']':
      CALL(handler.EndArray());
      return;
    default:
      throw Exception(error::MISS_COMMA_OR_SQUARE_BRACKET);
    }
  }
}

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseObject(ReadStream &rs, Handler &handler) {
  CALL(handler.StartObject());

  rs.assertNext('{');
  ParseWhitespace(rs);
  if (rs.peek() == '}') {
    rs.next();
    CALL(handler.EndObject());
    return;
  }

  while (true) {
    if (rs.peek() != '"') {
      throw Exception(error::MISS_KEY);
    }

    ParseString(rs, handler, true);

    // parse ':'
    ParseWhitespace(rs);
    if (rs.next() != ':') {
      throw Exception(error::MISS_COLON);
    }
    ParseWhitespace(rs);

    // go on
    ParseValue(rs, handler);
    ParseWhitespace(rs);
    switch (rs.next()) {
    case ',':
      ParseWhitespace(rs);
      break;
    case '}':
      CALL(handler.EndObject());
      return;
    default:
      throw Exception(error::MISS_COMMA_OR_CURLY_BRACKET);
    }
  }
}

#undef CALL

template <required::read_stream::HasAllRequiredFunctions ReadStream,
          required::handler::HasAllRequiredFunctions Handler>
void Reader::ParseValue(ReadStream &rs, Handler &handler) {
  if (!rs.hasNext()) {
    throw Exception(error::EXPECT_VALUE);
  }

  switch (rs.peek()) {
  case 'n':
    return ParseLiteral(rs, handler, "null", NEU_NULL);
  case 't':
    return ParseLiteral(rs, handler, "true", NEU_BOOL);
  case 'f':
    return ParseLiteral(rs, handler, "false", NEU_BOOL);
  case '"':
    return ParseString(rs, handler, false);
  case '[':
    return ParseArray(rs, handler);
  case '{':
    return ParseObject(rs, handler);
  default:
    return ParseNumber(rs, handler);
  }
}

inline void Reader::EncodeUtf8(std::string &buffer, const unsigned int u) {
  if (u <= 0x7F) {
    buffer.push_back(static_cast<char>(u & 0xFF));
  } else if (u <= 0x7FF) {
    buffer.push_back(static_cast<char>(0xC0 | ((u >> 6) & 0xFF)));
    buffer.push_back(static_cast<char>(0x80 | (u & 0x3F)));
  } else if (u <= 0xFFFF) {
    buffer.push_back(static_cast<char>(0xE0 | ((u >> 12) & 0xFF)));
    buffer.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
    buffer.push_back(static_cast<char>(0x80 | (u & 0x3F)));
  } else if (u <= 0x10FFFF) {
    NEUJSON_ASSERT(u <= 0x10FFFF && "out of range");
    buffer.push_back(static_cast<char>(0xF0 | ((u >> 18) & 0xFF)));
    buffer.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3F)));
    buffer.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
    buffer.push_back(static_cast<char>(0x80 | (u & 0x3F)));
  }
}

} // namespace neujson

#if defined(NEUJSON_SSE42_OFF)
#define NEUJSON_SSE42
#elif defined(NEUJSON_SSE2_OFF)
#define NEUJSON_SSE2
#elif defined(NEUJSON_NEON_OFF)
#define NEUJSON_NEON
#endif

#endif // NEUJSON_NEUJSON_READER_H_
