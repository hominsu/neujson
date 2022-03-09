//
// Created by Homin Su on 2022/3/7.
//

#include "reader.h"

void neujson::Reader::encodeUtf8(std::string &_buffer, unsigned int _u) {
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
    default: assert(false && "out of range");
  }
}
