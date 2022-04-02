//
// Created by Homin Su on 2022/3/7.
//
#if defined(_WINDOWS) && defined(Debug)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "neujson/reader.h"

void neujson::Reader::encodeUtf8(::std::string &_buffer, unsigned int _u) {
#if defined(_MSC_VER)
  if (_u >= 0x0 && _u <= 0x7F) {
    _buffer.push_back(_u & 0xFF);
  } else if (_u >= 0x080 && _u <= 0x7FF) {
    _buffer.push_back(0xC0 | ((_u >> 6) & 0xFF));
    _buffer.push_back(0x80 | (_u & 0x3F));
  } else if (_u >= 0x0800 && _u <= 0xFFFF) {
    _buffer.push_back(0xE0 | ((_u >> 12) & 0xFF));
    _buffer.push_back(0x80 | ((_u >> 6) & 0x3F));
    _buffer.push_back(0x80 | (_u & 0x3F));
  } else if (_u >= 0x010000 && _u <= 0x10FFFF) {
    _buffer.push_back(0xF0 | ((_u >> 18) & 0xFF));
    _buffer.push_back(0x80 | ((_u >> 12) & 0x3F));
    _buffer.push_back(0x80 | ((_u >> 6) & 0x3F));
    _buffer.push_back(0x80 | (_u & 0x3F));
  }
#elif defined(__clang__) || defined(__GNUC__)
  switch (_u) {
    case 0x00 ... 0x7F:_buffer.push_back(_u & 0xFF);
      break;
    case 0x080 ... 0x7FF:_buffer.push_back(0xC0 | ((_u >> 6) & 0xFF));
      _buffer.push_back(0x80 | (_u & 0x3F));
      break;
    case 0x0800 ... 0xFFFF:_buffer.push_back(0xE0 | ((_u >> 12) & 0xFF));
      _buffer.push_back(0x80 | ((_u >> 6) & 0x3F));
      _buffer.push_back(0x80 | (_u & 0x3F));
      break;
    case 0x010000 ... 0x10FFFF:_buffer.push_back(0xF0 | ((_u >> 18) & 0xFF));
      _buffer.push_back(0x80 | ((_u >> 12) & 0x3F));
      _buffer.push_back(0x80 | ((_u >> 6) & 0x3F));
      _buffer.push_back(0x80 | (_u & 0x3F));
      break;
    default: NEUJSON_ASSERT(false && "out of range");
  }
#endif
}
