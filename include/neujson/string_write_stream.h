//
// Created by Homing So on 2022/3/10.
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
  static constexpr std::size_t kInnerBufferSize = 256;
  std::vector<Ch> buffer_;

public:
  StringWriteStream() { buffer_.reserve(kInnerBufferSize); }

  void put(const Ch ch) { buffer_.push_back(ch); }

  void puts(const Ch *str, const std::size_t length) {
    buffer_.insert(buffer_.end(), str, str + length);
  }

  void put_sv(const std::string_view str) {
    buffer_.insert(buffer_.end(), str.begin(), str.end());
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

#endif // NEUJSON_NEUJSON_STRING_WRITE_STREAM_H_
