//
// Created by Homin Su on 2022/3/7.
//

#ifndef NEUJSON_NEUJSON_READER_H_
#define NEUJSON_NEUJSON_READER_H_

#include <neujson/noncopyable.h>
#include <neujson/exception.h>
#include <neujson/value.h>

#include <cmath>

#include <exception>

namespace neujson {

class Reader : noncopyable {
 public:
  template<typename ReadStream, typename Handler>
  static error::ParseError parse(ReadStream &_rs, Handler &_handler);

 private:
  template<typename ReadStream>
  static unsigned parseHex4(ReadStream &_rs);

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
  static void encodeUtf8(std::string &_buffer, unsigned _u);
};

template<typename ReadStream, typename Handler>
error::ParseError Reader::parse(ReadStream &_rs, Handler &_handler) {
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
unsigned Reader::parseHex4(ReadStream &_rs) {
  unsigned u = 0;
  for (int i = 0; i < 4; i++) {
    u <<= 4;
    switch (char ch = _rs.next()) {
      case '0'...'9': u |= ch - '0';
        break;
      case 'a'...'f': u |= ch - 'a' + 10;
        break;
      case 'A'...'F': u |= ch - 'A' + 10;
        break;
      default: throw Exception(error::PARSE_BAD_UNICODE_HEX);
    }
  }
  return u;
}

template<typename ReadStream>
void Reader::parseWhitespace(ReadStream &_rs) {
  while (_rs.hasNext()) {
    char ch = _rs.peek();
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') { _rs.next(); }
    else { break; }
  }
}

#define CALL(_expr) if (!(_expr)) throw Exception(error::PARSE_USER_STOPPED)

template<typename ReadStream, typename Handler>
void Reader::parseLiteral(ReadStream &_rs, Handler &_handler, const char *_literal, Type _type) {
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
    case NEU_DOUBLE:CALL(_handler.Double(c == 'N' ? NAN : INFINITY));
      return;
    default:assert(false && "bad type");
  }
}

template<typename ReadStream, typename Handler>
void Reader::parseNumber(ReadStream &_rs, Handler &_handler) {
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
    std::size_t idx;
    if (expectType == NEU_DOUBLE) {
      double d;
#if defined(__clang__) || defined(_MSC_VER)
      d = ::std::stod(::std::string(start, end), &idx);
#elif defined(__GNUC__)
      d = __gnu_cxx::__stoa(&std::strtod, "stod", &*start, &idx);
#else
      assert(false && "complier no support");
#endif
      assert(start + idx == end);
      CALL(_handler.Double(d));
    } else {
      int64_t i64;
#if defined(__clang__) || defined(_MSC_VER)
      ::std::stol(::std::string(start, end), &idx, 10);
#elif defined(__GNUC__)
      i64 = __gnu_cxx::__stoa(&std::strtol, "stol", &*start, &idx, 10);
#else
      assert(false && "complier no support");
#endif
      if (i64 <= std::numeric_limits<int32_t>::max() && i64 >= std::numeric_limits<int32_t>::min()) {
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
void Reader::parseString(ReadStream &_rs, Handler &_handler, bool _is_key) {
  _rs.assertNext('"');
  std::string buffer;
  while (_rs.hasNext()) {
    switch (char ch = _rs.next()) {
      case '"':
        if (_is_key) { CALL(_handler.Key(std::move(buffer))); }
        else { CALL(_handler.String(std::move(buffer))); }
        return;
      case '\x01'...'\x1f':throw Exception(error::PARSE_BAD_STRING_CHAR);
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
      default: buffer.push_back(ch);
    }
  }
  throw Exception(error::PARSE_MISS_QUOTATION_MARK);
}

template<typename ReadStream, typename Handler>
void Reader::parseArray(ReadStream &_rs, Handler &_handler) {
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
void Reader::parseObject(ReadStream &_rs, Handler &_handler) {
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
void Reader::parseValue(ReadStream &_rs, Handler &_handler) {
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
