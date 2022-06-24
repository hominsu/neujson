//
// Created by HominSu on 2022/3/10.
//

#ifndef NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_
#define NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_

#include "neujson/noncopyable.h"

#include <vector>
#include <string_view>

namespace neujson {

class StringWriteStream : public noncopyable {
 private:
  ::std::vector<char> buffer_;

 public:
  void put(char _ch) {
    buffer_.emplace_back(_ch);
  }

  void puts(const char *_str, ::std::size_t _length) {
    buffer_.insert(buffer_.end(), _str, _str + _length);
  }

  void put_sv(::std::string_view _str) {
    buffer_.insert(buffer_.end(), _str.begin(), _str.end());
  }

  [[nodiscard]] ::std::string_view get() const {
    return {buffer_.data(), buffer_.size()};
  }

  void flush() {}

};

} // namespace neujson

#endif //NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_
