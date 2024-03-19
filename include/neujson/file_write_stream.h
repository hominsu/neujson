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
 private:
  static const std::size_t kInnerBufferSize = 256;
  std::FILE *fp_;
  char inner_buffer_[kInnerBufferSize]{};
  char *buffer_;
  char *buffer_end_;
  char *current_;

 public:
  explicit FileWriteStream(std::FILE *_fp)
      : fp_(_fp),
        buffer_(inner_buffer_),
        buffer_end_(buffer_ + kInnerBufferSize),
        current_(buffer_) {
    NEUJSON_ASSERT(_fp != nullptr && "FILE pointer equal zero");
  }

  explicit FileWriteStream(std::FILE *_fp, char *_buffer, std::size_t _buffer_size)
      : fp_(_fp),
        buffer_(_buffer),
        buffer_end_(_buffer + _buffer_size),
        current_(buffer_) {
    NEUJSON_ASSERT(_fp != nullptr && "FILE pointer equal zero");
  }

  ~FileWriteStream() { if (current_ != buffer_) { flush(); }}

  void put(char _ch) {
    if (current_ >= buffer_end_) { flush(); }
    *current_++ = _ch;
  }

  void put_n(char _ch, std::size_t _n) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    while (_n > avail) {
      std::memset(current_, _ch, avail);
      current_ += avail;
      flush();
      _n -= avail;
      avail = static_cast<size_t>(buffer_end_ - current_);
    }

    if (_n > 0) {
      std::memset(current_, _ch, _n);
      current_ += _n;
    }
  }

  void puts(const char *_str, std::size_t _length) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    std::size_t copy = 0;

    while (_length > avail) {
      std::memcpy(current_, _str + copy, avail);
      current_ += avail;
      copy += avail;
      flush();
      _length -= avail;
      avail = static_cast<std::size_t>(buffer_end_ - current_);
    }

    if (_length > 0) {
      std::memcpy(current_, _str + copy, _length);
      current_ += _length;
    }
  }

  void put_sv(std::string_view _sv) {
    auto avail = static_cast<std::size_t>(buffer_end_ - current_);
    std::size_t length = _sv.length();
    std::size_t copy = 0;

    while (length > avail) {
      std::memcpy(current_, _sv.data() + copy, avail);
      current_ += avail;
      copy += avail;
      flush();
      length -= avail;
      avail = static_cast<std::size_t>(buffer_end_ - current_);
    }

    if (length > 0) {
      std::memcpy(current_, _sv.data() + copy, length);
      current_ += length;
    }
  }

  void flush() {
    if (current_ != buffer_) {
      std::size_t result = std::fwrite(buffer_, 1, static_cast<std::size_t>(current_ - buffer_), fp_);
      if (result < static_cast<std::size_t>(current_ - buffer_)) {
      }
      current_ = buffer_;
    }
  }
};

} // namespace neujson

#endif //NEUJSON_NEUJSON_FILE_WRITE_STREAM_H_
