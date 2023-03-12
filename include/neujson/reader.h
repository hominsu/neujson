//
// Created by Homin Su on 2022/3/7.
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

class Reader : NonCopyable {
 public:
  template<typename ReadStream, typename Handler>
  static error::ParseError Parse(ReadStream &_rs, Handler &_handler);

 private:
  template<typename ReadStream>
  static unsigned ParseHex4(ReadStream &_rs);

#if defined(NEUJSON_SSE42)
  template<typename ReadStream>
  static void ParseWhitespaceSSE42(ReadStream &_rs);
#elif defined(NEUJSON_SSE2)
  template<typename ReadStream>
  static void ParseWhitespaceSSE2(ReadStream &_rs);
#elif defined(NEUJSON_NEON)
  template<typename ReadStream>
  static void ParseWhitespaceNEON(ReadStream &_rs);
#else
  template<typename ReadStream>
  static void ParseWhitespaceBasic(ReadStream &_rs);
#endif

  template<typename ReadStream>
  static void ParseWhitespace(ReadStream &_rs);

  template<typename ReadStream, typename Handler>
  static void ParseLiteral(ReadStream &_rs, Handler &_handler, const char *_literal, Type _type);

  template<typename ReadStream, typename Handler>
  static void ParseNumber(ReadStream &_rs, Handler &_handler);

  template<typename ReadStream, typename Handler>
  static void ParseString(ReadStream &_rs, Handler &_handler, bool _is_key);

  template<typename ReadStream, typename Handler>
  static void ParseArray(ReadStream &_rs, Handler &_handler);

  template<typename ReadStream, typename Handler>
  static void ParseObject(ReadStream &_rs, Handler &_handler);

  template<typename ReadStream, typename Handler>
  static void ParseValue(ReadStream &_rs, Handler &_handler);

 private:
  static bool IsDigit(char _ch) { return _ch >= '0' && _ch <= '9'; }
  static bool IsDigit1To9(char _ch) { return _ch >= '1' && _ch <= '9'; }
  static void EncodeUtf8(std::string &_buffer, unsigned int _u);
};

template<typename ReadStream, typename Handler>
inline error::ParseError Reader::Parse(ReadStream &_rs, Handler &_handler) {
  try {
    ParseWhitespace(_rs);
    ParseValue(_rs, _handler);
    ParseWhitespace(_rs);
    if (_rs.hasNext()) { throw Exception(error::ROOT_NOT_SINGULAR); }
    return error::OK;
  } catch (Exception &e) {
    return e.err();
  }
}

template<typename ReadStream>
inline unsigned Reader::ParseHex4(ReadStream &_rs) {
  unsigned int u = 0;
  for (int i = 0; i < 4; i++) {
    u <<= 4;
    char ch = _rs.next();
    if (ch >= '0' && ch <= '9') { u |= static_cast<unsigned int>(ch - '0'); }
    else if (ch >= 'a' && ch <= 'f') { u |= static_cast<unsigned int>(ch - 'a' + 10); }
    else if (ch >= 'A' && ch <= 'F') { u |= static_cast<unsigned int>(ch - 'A' + 10); }
    else { throw Exception(error::BAD_UNICODE_HEX); }
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
inline void Reader::ParseWhitespaceSSE42(ReadStream &_rs) {
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
    } else { return; }
  }

  // The rest of string using SIMD
  static const char whitespace[16] = " \n\r\t";
  const __m128i w = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespace[0]));

  for (;; p += 16, _rs.next(16)) {
    const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
    const int r =
        _mm_cmpistri(w, s, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
    // some characters are non-whitespace
    if (r != 16) {
      _rs.next(r);
      return;
    }
  }
}
#elif defined(NEUJSON_SSE2)
/**
 * @brief Skip whitespace with SSE2 instructions, testing 16 8-byte characters at once.
 * @tparam ReadStream
 * @param _rs
 */
template<typename ReadStream>
inline void Reader::ParseWhitespaceSSE2(ReadStream &_rs) {
  // Fast return for single non-whitespace
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
    } else { return; }
  }

  // The rest of string
#define C16(c) { c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c }
  static const char whitespaces[4][16] = {C16(' '), C16('\n'), C16('\r'), C16('\t')};
#undef C16

  const __m128i w0 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[0][0]));
  const __m128i w1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[1][0]));
  const __m128i w2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[2][0]));
  const __m128i w3 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&whitespaces[3][0]));

  for (;; p += 16, _rs.next(16)) {
    const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
    __m128i x = _mm_cmpeq_epi8(s, w0);
    x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w1));
    x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w2));
    x = _mm_or_si128(x, _mm_cmpeq_epi8(s, w3));
    auto r = static_cast<unsigned short>(~_mm_movemask_epi8(x));
    if (r != 0) { // some characters may be non-whitespace
#ifdef _MSC_VER   // Find the index of first non-whitespace
      unsigned long offset;
      _BitScanForward(&offset, r);
      _rs.next(offset);
      return;
#else
      _rs.next(__builtin_ffs(r) - 1);
      return;
#endif
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
inline void Reader::ParseWhitespaceNEON(ReadStream &_rs) {
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
inline void Reader::ParseWhitespaceBasic(ReadStream &_rs) {
  while (_rs.hasNext()) {
    char ch = _rs.peek();
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') { _rs.next(); }
    else { break; }
  }
}
#endif

template<typename ReadStream>
inline void Reader::ParseWhitespace(ReadStream &_rs) {
#if defined(NEUJSON_SSE42)
  return ParseWhitespaceSSE42(_rs);
#elif defined(NEUJSON_SSE2)
  return ParseWhitespaceSSE2(_rs);
#elif defined(NEUJSON_NEON)
  return ParseWhitespaceNEON(_rs);
#else
  return ParseWhitespaceBasic(_rs);
#endif
}

#define CALL(_expr) if (!(_expr)) throw Exception(error::USER_STOPPED)

template<typename ReadStream, typename Handler>
inline void Reader::ParseLiteral(ReadStream &_rs, Handler &_handler, const char *_literal, Type _type) {
  char c = *_literal;

  _rs.assertNext(*_literal++);
  for (; *_literal != '\0'; _literal++, _rs.next()) {
    if (*_literal != _rs.peek()) { throw Exception(error::BAD_VALUE); }
  }

  switch (_type) {
    case NEU_NULL:CALL(_handler.Null());
      return;
    case NEU_BOOL:CALL(_handler.Bool(c == 't'));
      return;
    case NEU_DOUBLE:CALL(_handler.Double(internal::Double(
          c == 'N' ? std::numeric_limits<double>::quiet_NaN() : std::numeric_limits<double>::infinity()
      )));
      return;
    default:NEUJSON_ASSERT(false && "bad type");
  }
}

template<typename ReadStream, typename Handler>
inline void Reader::ParseNumber(ReadStream &_rs, Handler &_handler) {
  // parse 'NaN' (Not a Number) && 'Infinity'
  if (_rs.peek() == 'N') {
    ParseLiteral(_rs, _handler, "NaN", NEU_DOUBLE);
    return;
  } else if (_rs.peek() == 'I') {
    ParseLiteral(_rs, _handler, "Infinity", NEU_DOUBLE);
    return;
  }

  std::string buffer;

  if (_rs.peek() == '-') { buffer.push_back(_rs.next()); }
  if (_rs.peek() == '0') { buffer.push_back(_rs.next()); }
  else {
    if (!IsDigit1To9(_rs.peek())) { throw Exception(error::BAD_VALUE); }
    for (buffer.push_back(_rs.next()); IsDigit(_rs.peek()); buffer.push_back(_rs.next()));
  }

  auto expectType = NEU_NULL;

  if (_rs.peek() == '.') {
    expectType = NEU_DOUBLE;
    buffer.push_back(_rs.next());
    if (!IsDigit(_rs.peek())) { throw Exception(error::BAD_VALUE); }
    for (buffer.push_back(_rs.next()); IsDigit(_rs.peek()); buffer.push_back(_rs.next()));
  }

  if (_rs.peek() == 'e' || _rs.peek() == 'E') {
    expectType = NEU_DOUBLE;
    buffer.push_back(_rs.next());
    if (_rs.peek() == '+' || _rs.peek() == '-') { buffer.push_back(_rs.next()); }
    if (!IsDigit(_rs.peek())) { throw Exception(error::BAD_VALUE); }
    for (buffer.push_back(_rs.next()); IsDigit(_rs.peek()); buffer.push_back(_rs.next()));
  }

  if (buffer.empty()) { throw Exception(error::BAD_VALUE); }

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
      CALL(_handler.Double(internal::Double(d)));
    } else {
      int64_t i64;
#if defined(__clang__) || defined(_MSC_VER)
      i64 = std::stoll(buffer, &idx, 10);
#elif defined(__GNUC__)
      i64 = __gnu_cxx::__stoa(&std::strtoll, "stoll", buffer.data(), &idx, 10);
#else
#error "complier no support"
#endif
      if (i64 <= std::numeric_limits<int32_t>::max() && i64 >= std::numeric_limits<int32_t>::min()) {
        CALL(_handler.Int32(static_cast<int32_t>(i64)));
      } else {
        CALL(_handler.Int64(i64));
      }
    }
  } catch (...) {
    throw Exception(error::NUMBER_TOO_BIG);
  }

}

template<typename ReadStream, typename Handler>
inline void Reader::ParseString(ReadStream &_rs, Handler &_handler, bool _is_key) {
  _rs.assertNext('"');
  std::string buffer;
  while (_rs.hasNext()) {
    switch (char ch = _rs.next()) {
      case '"':
        if (_is_key) { CALL(_handler.Key(std::move(buffer))); }
        else { CALL(_handler.String(std::move(buffer))); }
        return;
#if defined(__clang__) || defined(__GNUC__)
      case '\x01'...'\x1f': throw Exception(error::BAD_STRING_CHAR);
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
            unsigned u = ParseHex4(_rs);
            if (u >= 0xD800 && u <= 0xDBFF) {
              if (_rs.next() != '\\') { throw Exception(error::BAD_UNICODE_SURROGATE); }
              if (_rs.next() != 'u') { throw Exception(error::BAD_UNICODE_SURROGATE); }
              unsigned u2 = ParseHex4(_rs);
              if (u2 >= 0xDC00 && u2 <= 0xDFFF) {
                u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
              } else {
                throw Exception(error::BAD_UNICODE_SURROGATE);
              }
            }
            EncodeUtf8(buffer, u);
            break;
          }
          default: throw Exception(error::BAD_STRING_ESCAPE);
        }
        break;
      default:
#if defined(_MSC_VER)
        if (static_cast<unsigned char>(ch) < 0x20) { throw Exception(error::BAD_STRING_CHAR); }
#endif
        buffer.push_back(ch);
    }
  }
  throw Exception(error::MISS_QUOTATION_MARK);
}

template<typename ReadStream, typename Handler>
inline void Reader::ParseArray(ReadStream &_rs, Handler &_handler) {
  CALL(_handler.StartArray());

  _rs.assertNext('[');
  ParseWhitespace(_rs);
  if (_rs.peek() == ']') {
    _rs.next();
    CALL(_handler.EndArray());
    return;
  }

  while (true) {
    ParseValue(_rs, _handler);
    ParseWhitespace(_rs);
    switch (_rs.next()) {
      case ',':ParseWhitespace(_rs);
        break;
      case ']':CALL(_handler.EndArray());
        return;
      default:throw Exception(error::MISS_COMMA_OR_SQUARE_BRACKET);
    }
  }
}

template<typename ReadStream, typename Handler>
inline void Reader::ParseObject(ReadStream &_rs, Handler &_handler) {
  CALL(_handler.StartObject());

  _rs.assertNext('{');
  ParseWhitespace(_rs);
  if (_rs.peek() == '}') {
    _rs.next();
    CALL(_handler.EndObject());
    return;
  }

  while (true) {
    if (_rs.peek() != '"') { throw Exception(error::MISS_KEY); }

    ParseString(_rs, _handler, true);

    // parse ':'
    ParseWhitespace(_rs);
    if (_rs.next() != ':') { throw Exception(error::MISS_COLON); }
    ParseWhitespace(_rs);

    // go on
    ParseValue(_rs, _handler);
    ParseWhitespace(_rs);
    switch (_rs.next()) {
      case ',':ParseWhitespace(_rs);
        break;
      case '}':CALL(_handler.EndObject());
        return;
      default:throw Exception(error::MISS_COMMA_OR_CURLY_BRACKET);
    }
  }
}

#undef CALL

template<typename ReadStream, typename Handler>
inline void Reader::ParseValue(ReadStream &_rs, Handler &_handler) {
  if (!_rs.hasNext()) { throw Exception(error::EXPECT_VALUE); }

  switch (_rs.peek()) {
    case 'n': return ParseLiteral(_rs, _handler, "null", NEU_NULL);
    case 't': return ParseLiteral(_rs, _handler, "true", NEU_BOOL);
    case 'f': return ParseLiteral(_rs, _handler, "false", NEU_BOOL);
    case '"': return ParseString(_rs, _handler, false);
    case '[': return ParseArray(_rs, _handler);
    case '{': return ParseObject(_rs, _handler);
    default: return ParseNumber(_rs, _handler);
  }
}

inline void Reader::EncodeUtf8(std::string &_buffer, unsigned int _u) {
  if (_u <= 0x7F) {
    _buffer.push_back(static_cast<char>(_u & 0xFF));
  } else if (_u <= 0x7FF) {
    _buffer.push_back(static_cast<char>(0xC0 | ((_u >> 6) & 0xFF)));
    _buffer.push_back(static_cast<char>(0x80 | (_u & 0x3F)));
  } else if (_u <= 0xFFFF) {
    _buffer.push_back(static_cast<char>(0xE0 | ((_u >> 12) & 0xFF)));
    _buffer.push_back(static_cast<char>(0x80 | ((_u >> 6) & 0x3F)));
    _buffer.push_back(static_cast<char>(0x80 | (_u & 0x3F)));
  } else if (_u <= 0x10FFFF) {
    NEUJSON_ASSERT(_u <= 0x10FFFF && "out of range");
    _buffer.push_back(static_cast<char>(0xF0 | ((_u >> 18) & 0xFF)));
    _buffer.push_back(static_cast<char>(0x80 | ((_u >> 12) & 0x3F)));
    _buffer.push_back(static_cast<char>(0x80 | ((_u >> 6) & 0x3F)));
    _buffer.push_back(static_cast<char>(0x80 | (_u & 0x3F)));
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

#endif //NEUJSON_NEUJSON_READER_H_
