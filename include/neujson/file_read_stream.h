//
// Created by Homing So on 2022/3/11.
//

#ifndef NEUJSON_NEUJSON_FILE_READ_STREAM_H_
#define NEUJSON_NEUJSON_FILE_READ_STREAM_H_

#include <cstdio>

#include "neujson.h"
#include "non_copyable.h"

namespace neujson {

class FileReadStream : NonCopyable {
  static constexpr std::size_t kInnerBufferSize = 256;
  std::FILE *fp_;
  char inner_buffer_[kInnerBufferSize]{};
  char *buffer_;
  char *current_;
  char *buffer_last_;
  std::size_t buffer_size_;
  std::size_t read_count_;
  std::size_t read_total_;
  bool eof_;

public:
  explicit FileReadStream(FILE *_fp)
      : fp_(_fp), buffer_(inner_buffer_), current_(inner_buffer_),
        buffer_last_(nullptr), buffer_size_(kInnerBufferSize), read_count_(0),
        read_total_(0), eof_(false) {
    NEUJSON_ASSERT(fp_ != nullptr && "file pointer should not be empty");
    read();
  }

  explicit FileReadStream(FILE *_fp, char *_buffer, const std::size_t _buffer_size)
      : fp_(_fp), buffer_(_buffer), current_(_buffer), buffer_last_(nullptr),
        buffer_size_(_buffer_size), read_count_(0), read_total_(0),
        eof_(false) {
    NEUJSON_ASSERT(fp_ != nullptr && "file pointer should not be empty");
    NEUJSON_ASSERT(buffer_size_ >= 4 &&
                   "buffer size should be bigger then four");
    read();
  }

  template <std::size_t N>
  explicit FileReadStream(FILE *_fp, char (&_buffer)[N])
      : fp_(_fp), buffer_(_buffer), current_(_buffer), buffer_last_(nullptr),
        buffer_size_(N), read_count_(0), read_total_(0), eof_(false) {
    NEUJSON_ASSERT(fp_ != nullptr && "file pointer should not be empty");
    NEUJSON_ASSERT(buffer_size_ >= 4 &&
                   "buffer size should be bigger then four");
    read();
  }

  [[nodiscard]] bool hasNext() const {
    return !eof_ || (current_ + 1 - !eof_ <= buffer_last_);
  }

  [[nodiscard]] char peek() const { return *current_; }

  char next() {
    const char ch = *current_;
    read();
    return ch;
  }

  template <typename T>
    requires std::is_integral_v<T>
  void next(T _n) {
    NEUJSON_ASSERT(_n >= 0);
    for (T i = 0; i < _n; ++i) {
      if (hasNext()) {
        read();
      } else {
        break;
      }
    }
  }

  void assertNext(const char _ch) {
    (void)_ch;
    NEUJSON_ASSERT(peek() == _ch);
    read();
  }

private:
  void read() {
    if (current_ < buffer_last_) {
      ++current_;
    } else if (!eof_) {
      read_total_ += read_count_;
      read_count_ = std::fread(buffer_, 1, buffer_size_, fp_);
      buffer_last_ = buffer_ + read_count_ - 1;
      current_ = buffer_;

      if (read_count_ < buffer_size_) {
        buffer_[read_count_] = '\0';
        ++buffer_last_;
        eof_ = true;
      }
    }
  }
};

} // namespace neujson

#endif // NEUJSON_NEUJSON_FILE_READ_STREAM_H_
