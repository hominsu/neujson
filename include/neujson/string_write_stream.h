//
// Created by HominSu on 2022/3/10.
//

#ifndef NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_
#define NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_

#include <string_view>
#include <vector>

#include "neujson.h"
#include "non_copyable.h"

#if defined(__GNUC__)
NEUJSON_DIAG_PUSH
NEUJSON_DIAG_OFF(effc++)
#endif

namespace neujson {

class StringWriteStream : public NonCopyable {
 public:
  using Ch = char;

 private:
  static const std::size_t kInnerBufferSize = 256;
  std::vector<Ch> buffer_;

 public:
  StringWriteStream() { buffer_.reserve(kInnerBufferSize); }

  void put(Ch _ch) {
    buffer_.push_back(_ch);
  }

  void puts(const Ch *_str, std::size_t _length) {
    buffer_.insert(buffer_.end(), _str, _str + _length);
  }

  void put_sv(std::string_view _str) {
    buffer_.insert(buffer_.end(), _str.begin(), _str.end());
  }

  [[nodiscard]] std::string_view get() const {
    return {buffer_.data(), buffer_.size()};
  }

  void flush() {}

};

} // namespace neujson

#if defined(__GNUC__)
NEUJSON_DIAG_POP
#endif

#endif //NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_
