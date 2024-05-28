//
// Created by Homing So on 2022/7/1.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_ISTREAM_WRAPPER_H_
#define NEUJSON_INCLUDE_NEUJSON_ISTREAM_WRAPPER_H_

#include <sstream>

#include "non_copyable.h"

namespace neujson {

namespace required {

template <typename Stream>
concept StreamCharTypeIsChar =
    requires { std::same_as<typename Stream::char_type, char>; };

} // namespace required

template <required::StreamCharTypeIsChar Stream>
class IStreamWrapper : NonCopyable {
  static constexpr std::size_t kInnerBufferSize = 256;
  Stream &stream_;
  char inner_buffer_[kInnerBufferSize]{};
  char *buffer_;
  char *current_;
  char *buffer_last_;
  std::size_t buffer_size_;
  std::size_t read_count_;
  std::size_t read_total_;
  bool eof_;

public:
  explicit IStreamWrapper(Stream &stream)
      : stream_(stream), buffer_(inner_buffer_), current_(inner_buffer_),
        buffer_last_(nullptr), buffer_size_(kInnerBufferSize), read_count_(0),
        read_total_(0), eof_(false) {
    read();
  }

  IStreamWrapper(Stream &stream, char *buffer, const std::size_t buffer_size)
      : stream_(stream), buffer_(buffer), current_(buffer_),
        buffer_last_(nullptr), buffer_size_(buffer_size), read_count_(0),
        read_total_(0), eof_(false) {
    NEUJSON_ASSERT(buffer_size_ >= 4 &&
                   "buffer size should be bigger then four");
    read();
  }

  template <std::size_t N>
  IStreamWrapper(Stream &stream, char (&buffer)[N])
      : stream_(stream), buffer_(buffer), current_(buffer_),
        buffer_last_(nullptr), buffer_size_(N), read_count_(0), read_total_(0),
        eof_(false) {
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

  std::string next(const std::size_t n) {
    auto str = std::make_shared<std::string>();
    auto ret = read(str, n);
    return {ret->c_str(), ret->size()};
  }

  void skip(const std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
      if (hasNext()) {
        read();
      } else {
        break;
      }
    }
  }

  void assertNext(const char ch) {
    (void)ch;
    NEUJSON_ASSERT(peek() == ch);
    read();
  }

private:
  void read() {
    if (current_ < buffer_last_) {
      ++current_;
    } else if (!eof_) {
      read_total_ += read_count_;

      // if no eof
      buffer_last_ = buffer_ + buffer_size_ - 1;
      current_ = buffer_;

      // eof
      if (!stream_.read(buffer_, static_cast<std::streamsize>(buffer_size_))) {
        read_count_ = static_cast<std::size_t>(stream_.gcount());
        *(buffer_last_ = buffer_ + read_count_) = '\0';
        eof_ = true;
      }
    }
  }

  std::shared_ptr<std::string> &read(std::shared_ptr<std::string> &str,
                                     const std::size_t n = 1) {
    if (current_ + n <= buffer_last_) {
      str->append(current_, n);
      current_ += n;
      return str;
    } else if (!eof_) {
      const std::size_t remaining = n - (buffer_last_ - current_ + 1);
      const std::size_t need_read = n - remaining;
      str->append(current_, remaining);

      read_total_ += read_count_;

      // if no eof
      buffer_last_ = buffer_ + buffer_size_ - 1;
      current_ = buffer_;

      // eof
      if (!stream_.read(buffer_, static_cast<std::streamsize>(buffer_size_))) {
        read_count_ = static_cast<std::size_t>(stream_.gcount());
        *(buffer_last_ = buffer_ + read_count_) = '\0';
        eof_ = true;
      }

      if (eof_ && need_read > read_count_) {
        str->append(current_, read_count_);
        return str;
      }

      return read(str, need_read);
    }

    return str;
  }
};

} // namespace neujson

#endif // NEUJSON_INCLUDE_NEUJSON_ISTREAM_WRAPPER_H_
