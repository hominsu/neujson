//
// Created by HominSu on 2022/3/10.
//

#ifndef NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_
#define NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_

#include <neujson/noncopyable.h>

#include <vector>
#include <string_view>

namespace neujson {

class StringWriteStream : public noncopyable {
 private:
  ::std::vector<char> buffer_;

 public:
  void put(char _ch) {
    buffer_.push_back(_ch);
  }

  void put(::std::string_view _str) {
    buffer_.insert(buffer_.end(), _str.begin(), _str.end());
  }

  [[nodiscard]] ::std::string_view get() const {
    return {buffer_.data(), buffer_.size()};
  }
};

} // namespace neujson

#endif //NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_
