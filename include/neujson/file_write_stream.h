//
// Created by Homing So on 2022/3/11.
//

#ifndef NEUJSON_NEUJSON_FILE_WRITE_STREAM_H_
#define NEUJSON_NEUJSON_FILE_WRITE_STREAM_H_

#include <cstdio>
#include <cstring>

#include <string_view>

#include "neujson.h"
#include "non_copyable.h"

namespace neujson {

class FileWriteStream : public NonCopyable {
  static constexpr std::size_t kInnerBufferSize = 256;
  std::FILE *fp_;
  char inner_buffer_[kInnerBufferSize]{};
  char *buffer_;
  char *buffer_end_;
  char *current_;

public:
  explicit FileWriteStream(std::FILE *fp)
      : fp_(fp), buffer_(inner_buffer_),
        buffer_end_(buffer_ + kInnerBufferSize), current_(buffer_) {
    NEUJSON_ASSERT(fp != nullptr && "FILE pointer equal zero");
  }

  explicit FileWriteStream(std::FILE *fp, char *buffer,
                           const std::size_t buffer_size)
      : fp_(fp), buffer_(buffer), buffer_end_(buffer + buffer_size),
        current_(buffer_) {
    NEUJSON_ASSERT(fp != nullptr && "FILE pointer equal zero");
  }

  template <std::size_t N>
  explicit FileWriteStream(std::FILE *fp, char (&buffer)[N])
      : fp_(fp), buffer_(buffer), buffer_end_(buffer + N),
        current_(buffer_) {
    NEUJSON_ASSERT(fp != nullptr && "FILE pointer equal zero");
  }

  ~FileWriteStream() {
    if (current_ != buffer_) {
      flush();
    }
  }

  void put(const char ch) {
    if (current_ >= buffer_end_) {
      flush();
    }
    *current_++ = ch;
  }

  void put_n(const char ch, std::size_t n) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    while (n > avail) {
      std::memset(current_, ch, avail);
      current_ += avail;
      flush();
      n -= avail;
      avail = static_cast<size_t>(buffer_end_ - current_);
    }

    if (n > 0) {
      std::memset(current_, ch, n);
      current_ += n;
    }
  }

  void puts(const char *str, std::size_t length) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    std::size_t copy = 0;

    while (length > avail) {
      std::memcpy(current_, str + copy, avail);
      current_ += avail;
      copy += avail;
      flush();
      length -= avail;
      avail = static_cast<std::size_t>(buffer_end_ - current_);
    }

    if (length > 0) {
      std::memcpy(current_, str + copy, length);
      current_ += length;
    }
  }

  void put_sv(const std::string_view sv) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    std::size_t length = sv.length();
    std::size_t copy = 0;

    while (length > avail) {
      std::memcpy(current_, sv.data() + copy, avail);
      current_ += avail;
      copy += avail;
      flush();
      length -= avail;
      avail = static_cast<std::size_t>(buffer_end_ - current_);
    }

    if (length > 0) {
      std::memcpy(current_, sv.data() + copy, length);
      current_ += length;
    }
  }

  void flush() {
    if (current_ != buffer_) {
      const std::size_t result = std::fwrite(
          buffer_, 1, static_cast<std::size_t>(current_ - buffer_), fp_);
      if (result < static_cast<std::size_t>(current_ - buffer_)) {
      }
      current_ = buffer_;
    }
  }
};

} // namespace neujson

#endif // NEUJSON_NEUJSON_FILE_WRITE_STREAM_H_
