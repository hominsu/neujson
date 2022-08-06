//
// Created by HominSu on 2022/7/1.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_ISTREAM_WRAPPER_H_
#define NEUJSON_INCLUDE_NEUJSON_ISTREAM_WRAPPER_H_

#include "sstream"

#include "noncopyable.h"

namespace neujson {

template<class Stream>
class IStreamWrapper : noncopyable {
 public:
  using Ch = typename Stream::char_type;

 private:
  static const ::std::size_t kInnerBufferSize = 256;
  Stream &stream_;
  Ch inner_buffer_[kInnerBufferSize]{};
  Ch *buffer_;
  Ch *current_;
  Ch *buffer_last_;
  ::std::size_t buffer_size_;
  ::std::size_t read_count_;
  ::std::size_t read_total_;
  bool eof_;

 public:
  explicit IStreamWrapper(Stream &_stream)
      : stream_(_stream),
        buffer_(inner_buffer_),
        current_(inner_buffer_),
        buffer_last_(nullptr),
        buffer_size_(kInnerBufferSize),
        read_count_(0),
        read_total_(0),
        eof_(false) {
    read();
  }

  IStreamWrapper(Stream &_stream, char *_buffer, ::std::size_t _buffer_size)
      : stream_(_stream),
        buffer_(_buffer),
        current_(buffer_),
        buffer_last_(nullptr),
        buffer_size_(_buffer_size),
        read_count_(0),
        read_total_(0),
        eof_(false) {
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
    for (Tint i = 0; i < _n; ++i) {
      if (hasNext()) { read(); }
      else { break; }
    }
  }

  void assertNext(char _ch) {
    (void) _ch;
    NEUJSON_ASSERT(peek() == _ch);
    read();
  }

 private:
  void read() {
    if (current_ < buffer_last_) { ++current_; }
    else if (!eof_) {
      read_total_ += read_count_;

      // if no eof
      buffer_last_ = buffer_ + buffer_size_ - 1;
      current_ = buffer_;

      // eof
      if (!stream_.read(buffer_, static_cast<::std::streamsize>(buffer_size_))) {
        read_count_ = static_cast<::std::size_t>(stream_.gcount());
        *(buffer_last_ = buffer_ + read_count_) = '\0';
        eof_ = true;
      }
    }
  }
};

} // namespace neujson

#endif //NEUJSON_INCLUDE_NEUJSON_ISTREAM_WRAPPER_H_
