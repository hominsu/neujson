//
// Created by HominSu on 2022/3/11.
//

#ifndef NEUJSON_NEUJSON_FILE_READ_STREAM_H_
#define NEUJSON_NEUJSON_FILE_READ_STREAM_H_

#include <cstdio>

#include <vector>

#include "neujson.h"
#include "noncopyable.h"

namespace neujson {

class FileReadStream : noncopyable {
 public:
  using Ch = char;

 private:
  static const ::std::size_t kInnerBufferSize = 256;
  ::std::FILE *fp_;
  Ch inner_buffer_[kInnerBufferSize]{};
  Ch *buffer_;
  Ch *current_;
  Ch *buffer_last_;
  ::std::size_t buffer_size_;
  ::std::size_t read_count_;
  ::std::size_t read_total_;
  bool eof_;

 public:
  explicit FileReadStream(FILE *_fp)
      : fp_(_fp),
        buffer_(inner_buffer_),
        current_(inner_buffer_),
        buffer_last_(nullptr),
        buffer_size_(kInnerBufferSize),
        read_count_(0),
        read_total_(0),
        eof_(false) {
    NEUJSON_ASSERT(fp_ != nullptr && "file pointer should not be empty");
    read();
  }

  explicit FileReadStream(FILE *_fp, char *_buffer, ::std::size_t _buffer_size)
      : fp_(_fp),
        buffer_(_buffer),
        current_(buffer_),
        buffer_last_(nullptr),
        buffer_size_(_buffer_size),
        read_count_(0),
        read_total_(0),
        eof_(false) {
    NEUJSON_ASSERT(fp_ != nullptr && "file pointer should not be empty");
    NEUJSON_ASSERT(buffer_size_ >= 4 && "buffer size should be bigger then four");
    read();
  }

  [[nodiscard]] bool hasNext() const { return !eof_ || (current_ + 1 - !eof_ <= buffer_last_); }

  Ch peek() { return *current_; }

  Ch next() {
    Ch ch = *current_;
    read();
    return ch;
  }

  template<typename Tint, class = typename std::enable_if_t<std::is_integral_v<std::remove_reference_t<Tint>>>>
  void next(Tint _n) {
    NEUJSON_ASSERT(_n >= 0);
    for (Tint i = 0; i < _n; ++i) {
      if (hasNext()) { read(); }
      else { break; }
    }
  }

  void assertNext(Ch _ch) {
    (void) _ch;
    NEUJSON_ASSERT(peek() == _ch);
    read();
  }

 private:
  void read() {
    if (current_ < buffer_last_) { ++current_; }
    else if (!eof_) {
      read_total_ += read_count_;
      read_count_ = ::std::fread(buffer_, 1, buffer_size_, fp_);
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

#endif //NEUJSON_NEUJSON_FILE_READ_STREAM_H_
