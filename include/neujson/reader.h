//
// Created by Homin Su on 2022/3/7.
//

#ifndef NEUJSON_NEUJSON_READER_H_
#define NEUJSON_NEUJSON_READER_H_

#include "noncopyable.h"
#include "exception.h"
#include "value.h"
#include "internal/cllzl.h"

#include <cmath>

#if defined(NEUJSON_SSE42)
#include <nmmintrin.h>
#elif defined(NEUJSON_NEON)
#include <arm_neon.h>
#endif

#include <exception>

namespace neujson {

class Reader : noncopyable {
 public:
  template<typename ReadStream, typename Handler>
  static error::ParseError parse(ReadStream &_rs, Handler &_handler);

 private:
  template<typename ReadStream>
  static unsigned parseHex4(ReadStream &_rs);

#if defined(NEUJSON_SSE42)
  template<typename ReadStream>
  static void parseWhitespace_SIMD_SSE42(ReadStream &_rs);
#elif defined(NEUJSON_NEON)
  template<typename ReadStream>
  static void parseWhitespace_SIMD_NEON(ReadStream &_rs);
#else
  template<typename ReadStream>
  static void parseWhitespace_BASIC(ReadStream &_rs);
#endif

  template<typename ReadStream>
  static void parseWhitespace(ReadStream &_rs);

  template<typename ReadStream, typename Handler>
  static void parseLiteral(ReadStream &_rs, Handler &_handler, const char *_literal, Type _type);

  template<typename ReadStream, typename Handler>
  static void parseNumber(ReadStream &_rs, Handler &_handler);

  template<typename ReadStream, typename Handler>
  static void parseString(ReadStream &_rs, Handler &_handler, bool _is_key);

  template<typename ReadStream, typename Handler>
  static void parseArray(ReadStream &_rs, Handler &_handler);

  template<typename ReadStream, typename Handler>
  static void parseObject(ReadStream &_rs, Handler &_handler);

  template<typename ReadStream, typename Handler>
  static void parseValue(ReadStream &_rs, Handler &_handler);

 private:
  static bool isDigit(char _ch) { return _ch >= '0' && _ch <= '9'; }
  static bool isDigit1to9(char _ch) { return _ch >= '1' && _ch <= '9'; }
  static void encodeUtf8(::std::string &_buffer, unsigned _u);
};

template<typename ReadStream, typename Handler>
inline error::ParseError Reader::parse(ReadStream &_rs, Handler &_handler) {
  try {
    parseWhitespace(_rs);
    parseValue(_rs, _handler);
    parseWhitespace(_rs);
    if (_rs.hasNext()) { throw Exception(error::PARSE_ROOT_NOT_SINGULAR); }
    return error::PARSE_OK;
  } catch (Exception &e) {
    return e.err();
  }
}

template<typename ReadStream>
inline unsigned Reader::parseHex4(ReadStream &_rs) {
  unsigned u = 0;
  for (int i = 0; i < 4; i++) {
    u <<= 4;
    char ch = _rs.next();
#if defined(_MSC_VER)
    if (ch >= '0' && ch <= '9') { u |= ch - '0'; }
    else if (ch >= 'a' && ch <= 'f') { u |= ch - 'a' + 10; }
    else if (ch >= 'A' && ch <= 'F') { u |= ch - 'A' + 10; }
    else { throw Exception(error::PARSE_BAD_UNICODE_HEX); }
#elif defined(__clang__) || defined(__GNUC__)
    switch (ch) {
      case '0'...'9': u |= ch - '0';
        break;
      case 'a'...'f': u |= ch - 'a' + 10;
        break;
      case 'A'...'F': u |= ch - 'A' + 10;
        break;
      default: throw Exception(error::PARSE_BAD_UNICODE_HEX);
    }
#endif
  }
  return u;
}

#if defined(NEUJSON_SSE42)
/**
 * @brief Skip whitespace with SSE 4.2 pcmpistrm instruction, testing 16 8-byte characters at once.
 * @tparam ReadStream
 * @param _rs
 */
template<typename ReadStream>
inline void Reader::parseWhitespace_SIMD_SSE42(ReadStream &_rs) {
  char ch = _rs.peek();
  if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') { _rs.next(); }
  else { return; }

  // 16-byte align to the next boundary
  const char *p = _rs.getAddr();
  const char
      *nextAligned = reinterpret_cast<const char *>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
  while (p != nextAligned)
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') { ++p; _rs.next(); }
    else { return; }
  }

  // The rest of string using SIMD
  static const char whitespace[16] = " \n\r\t";
  const __m128i w = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespace[0]));

  for (;; p += 16, _rs.next(16)) {
    const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
    const int r = _mm_cmpistri(w, s, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
    // some characters are non-whitespace
    if (r != 16) {
      _rs.next(r);
      return;
    }
  }
}
#elif defined(NEUJSON_NEON)
/**
 * @brief Skip whitespace with ARM Neon instructions, testing 16 8-byte characters at once.
 * @tparam ReadStream
 * @param _rs
 */
template<typename ReadStream>
inline void Reader::parseWhitespace_SIMD_NEON(ReadStream &_rs) {
  char ch = _rs.peek();
  if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') { _rs.next(); }
  else { return; }

  // 16-byte align to the next boundary
  const char *p = _rs.getAddr();
  const char
      *nextAligned = reinterpret_cast<const char *>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
  while (p != nextAligned) {
    if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') {
      ++p;
      _rs.next();
    }
    else { return; }
  }

  const uint8x16_t w0 = vmovq_n_u8(' ');
  const uint8x16_t w1 = vmovq_n_u8('\n');
  const uint8x16_t w2 = vmovq_n_u8('\r');
  const uint8x16_t w3 = vmovq_n_u8('\t');

  for (;; p += 16, _rs.next(16)) {
    const uint8x16_t s = vld1q_u8(reinterpret_cast<const uint8_t *>(p));
    uint8x16_t x = vceqq_u8(s, w0);
    x = vorrq_u8(x, vceqq_u8(s, w1));
    x = vorrq_u8(x, vceqq_u8(s, w2));
    x = vorrq_u8(x, vceqq_u8(s, w3));

    x = vmvnq_u8(x);                       // Negate
    x = vrev64q_u8(x);                     // Rev in 64
    uint64_t low = vgetq_lane_u64(vreinterpretq_u64_u8(x), 0);   // extract
    uint64_t high = vgetq_lane_u64(vreinterpretq_u64_u8(x), 1);  // extract

    if (low == 0) {
      if (high != 0) {
        uint32_t lz = internal::clzll(high);
        _rs.next(8 + (lz >> 3));
        return;
      }
    } else {
      uint32_t lz = internal::clzll(low);
      _rs.next(lz >> 3);
      return;
    }
  }
}
#else
template<typename ReadStream>
inline void Reader::parseWhitespace_BASIC(ReadStream &_rs) {
  while (_rs.hasNext()) {
    char ch = _rs.peek();
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') { _rs.next(); }
    else { break; }
  }
}
#endif

template<typename ReadStream>
inline void Reader::parseWhitespace(ReadStream &_rs) {
#if defined(NEUJSON_SSE42)
  return parseWhitespace_SIMD_SSE42(_rs);
#elif defined(NEUJSON_NEON)
  return parseWhitespace_SIMD_NEON(_rs);
#else
  return parseWhitespace_BASIC(_rs);
#endif
}

#define CALL(_expr) if (!(_expr)) throw Exception(error::PARSE_USER_STOPPED)

template<typename ReadStream, typename Handler>
inline void Reader::parseLiteral(ReadStream &_rs, Handler &_handler, const char *_literal, Type _type) {
  char c = *_literal;

  _rs.assertNext(*_literal++);
  for (; *_literal != '\0'; _literal++, _rs.next()) {
    if (*_literal != _rs.peek()) { throw Exception(error::PARSE_BAD_VALUE); }
  }

  switch (_type) {
    case NEU_NULL:CALL(_handler.Null());
      return;
    case NEU_BOOL:CALL(_handler.Bool(c == 't'));
      return;
    case NEU_DOUBLE:CALL(_handler.Double(
          c == 'N' ? std::numeric_limits<double>::quiet_NaN() : std::numeric_limits<double>::infinity()));
      return;
    default:NEUJSON_ASSERT(false && "bad type");
  }
}

template<typename ReadStream, typename Handler>
inline void Reader::parseNumber(ReadStream &_rs, Handler &_handler) {
  // parse 'NaN' (Not a Number) && 'Infinity'
  if (_rs.peek() == 'N') {
    parseLiteral(_rs, _handler, "NaN", NEU_DOUBLE);
    return;
  } else if (_rs.peek() == 'I') {
    parseLiteral(_rs, _handler, "Infinity", NEU_DOUBLE);
    return;
  }

  auto start = _rs.getIter();

  if (_rs.peek() == '-') { _rs.next(); }
  if (_rs.peek() == '0') { _rs.next(); }
  else {
    if (!isDigit1to9(_rs.peek())) { throw Exception(error::PARSE_BAD_VALUE); }
    for (_rs.next(); isDigit(_rs.peek()); _rs.next());
  }

  auto expectType = NEU_NULL;

  if (_rs.peek() == '.') {
    expectType = NEU_DOUBLE;
    _rs.next();
    if (!isDigit(_rs.peek())) { throw Exception(error::PARSE_BAD_VALUE); }
    for (_rs.next(); isDigit(_rs.peek()); _rs.next());
  }

  if (_rs.peek() == 'e' || _rs.peek() == 'E') {
    expectType = NEU_DOUBLE;
    _rs.next();
    if (_rs.peek() == '+' || _rs.peek() == '-') { _rs.next(); }
    if (!isDigit(_rs.peek())) { throw Exception(error::PARSE_BAD_VALUE); }
    for (_rs.next(); isDigit(_rs.peek()); _rs.next());
  }

  auto end = _rs.getIter();
  if (start == end) { throw Exception(error::PARSE_BAD_VALUE); }

  try {
    ::std::size_t idx;
    if (expectType == NEU_DOUBLE) {
      double d;
#if defined(__clang__) || defined(_MSC_VER)
      d = ::std::stod(::std::string(start, end), &idx);
#elif defined(__GNUC__)
      d = __gnu_cxx::__stoa(&::std::strtod, "stod", &*start, &idx);
#else
#error "complier no support"
#endif
      NEUJSON_ASSERT(start + idx == end);
      CALL(_handler.Double(d));
    } else {
      int64_t i64;
#if defined(__clang__) || defined(_MSC_VER)
      i64 = ::std::stoll(::std::string(start, end), &idx, 10);
#elif defined(__GNUC__)
      i64 = __gnu_cxx::__stoa(&::std::strtoll, "stoll", &*start, &idx, 10);
#else
#error "complier no support"
#endif
      if (i64 <= ::std::numeric_limits<int32_t>::max() && i64 >= ::std::numeric_limits<int32_t>::min()) {
        CALL(_handler.Int32(static_cast<int32_t>(i64)));
      } else {
        CALL(_handler.Int64(i64));
      }
    }
  } catch (::std::exception &_e) {
    throw Exception(error::PARSE_NUMBER_TOO_BIG);
  }

}

template<typename ReadStream, typename Handler>
inline void Reader::parseString(ReadStream &_rs, Handler &_handler, bool _is_key) {
  _rs.assertNext('"');
  ::std::string buffer;
  while (_rs.hasNext()) {
    switch (char ch = _rs.next()) {
      case '"':
        if (_is_key) { CALL(_handler.Key(::std::move(buffer))); }
        else { CALL(_handler.String(::std::move(buffer))); }
        return;
#if defined(__clang__) || defined(__GNUC__)
      case '\x01'...'\x1f':throw Exception(error::PARSE_BAD_STRING_CHAR);
#endif
      case '\\':
        switch (_rs.next()) {
          case '"': buffer.push_back('"');
            break;
          case '\\': buffer.push_back('\\');
            break;
          case '/': buffer.push_back('/');
            break;
          case 'b': buffer.push_back('\b');
            break;
          case 'f': buffer.push_back('\f');
            break;
          case 'n': buffer.push_back('\n');
            break;
          case 'r': buffer.push_back('\r');
            break;
          case 't': buffer.push_back('\t');
            break;
          case 'u': {
            // unicode stuff from Milo's tutorial
            unsigned u = parseHex4(_rs);
            if (u >= 0xD800 && u <= 0xDBFF) {
              if (_rs.next() != '\\') { throw Exception(error::PARSE_BAD_UNICODE_SURROGATE); }
              if (_rs.next() != 'u') { throw Exception(error::PARSE_BAD_UNICODE_SURROGATE); }
              unsigned u2 = parseHex4(_rs);
              if (u2 >= 0xDC00 && u2 <= 0xDFFF) {
                u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
              } else {
                throw Exception(error::PARSE_BAD_UNICODE_SURROGATE);
              }
            }
            encodeUtf8(buffer, u);
            break;
          }
          default: throw Exception(error::PARSE_BAD_STRING_ESCAPE);
        }
        break;
      default:
#if defined(_MSC_VER)
        if (static_cast<unsigned char>(ch) < 0x20) { throw Exception(error::PARSE_BAD_STRING_CHAR); }
#endif
        buffer.push_back(ch);
    }
  }
  throw Exception(error::PARSE_MISS_QUOTATION_MARK);
}

template<typename ReadStream, typename Handler>
inline void Reader::parseArray(ReadStream &_rs, Handler &_handler) {
  CALL(_handler.StartArray());

  _rs.assertNext('[');
  parseWhitespace(_rs);
  if (_rs.peek() == ']') {
    _rs.next();
    CALL(_handler.EndArray());
    return;
  }

  while (true) {
    parseValue(_rs, _handler);
    parseWhitespace(_rs);
    switch (_rs.next()) {
      case ',':parseWhitespace(_rs);
        break;
      case ']':CALL(_handler.EndArray());
        return;
      default:throw Exception(error::PARSE_MISS_COMMA_OR_SQUARE_BRACKET);
    }
  }
}

template<typename ReadStream, typename Handler>
inline void Reader::parseObject(ReadStream &_rs, Handler &_handler) {
  CALL(_handler.StartObject());

  _rs.assertNext('{');
  parseWhitespace(_rs);
  if (_rs.peek() == '}') {
    _rs.next();
    CALL(_handler.EndObject());
    return;
  }

  while (true) {
    if (_rs.peek() != '"') { throw Exception(error::PARSE_MISS_KEY); }

    parseString(_rs, _handler, true);

    // parse ':'
    parseWhitespace(_rs);
    if (_rs.next() != ':') { throw Exception(error::PARSE_MISS_COLON); }
    parseWhitespace(_rs);

    // go on
    parseValue(_rs, _handler);
    parseWhitespace(_rs);
    switch (_rs.next()) {
      case ',':parseWhitespace(_rs);
        break;
      case '}':CALL(_handler.EndObject());
        return;
      default:throw Exception(error::PARSE_MISS_COMMA_OR_CURLY_BRACKET);
    }
  }
}

#undef CALL

template<typename ReadStream, typename Handler>
inline void Reader::parseValue(ReadStream &_rs, Handler &_handler) {
  if (!_rs.hasNext()) { throw Exception(error::PARSE_EXPECT_VALUE); }

  switch (_rs.peek()) {
    case 'n': return parseLiteral(_rs, _handler, "null", NEU_NULL);
    case 't': return parseLiteral(_rs, _handler, "true", NEU_BOOL);
    case 'f': return parseLiteral(_rs, _handler, "false", NEU_BOOL);
    case '"': return parseString(_rs, _handler, false);
    case '[': return parseArray(_rs, _handler);
    case '{': return parseObject(_rs, _handler);
    default: return parseNumber(_rs, _handler);
  }
}

} // namespace neujson

#endif //NEUJSON_NEUJSON_READER_H_
