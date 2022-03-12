//
// Created by HominSu on 2022/3/11.
//

#ifndef NEUJSON_NEUJSON_FILE_READ_STREAM_H_
#define NEUJSON_NEUJSON_FILE_READ_STREAM_H_

#include "noncopyable.h"

#include <cstdio>
#include <cassert>

#include <vector>

namespace neujson {

class FileReadStream : noncopyable {
 public:
  using Iterator = ::std::vector<char>::iterator;

 private:
  std::vector<char> buffer_;
  Iterator iter_;

 public:
  explicit FileReadStream(FILE *_in) {
    readStream(_in);
    iter_ = buffer_.begin();
  }

  [[nodiscard]] bool hasNext() const { return iter_ != buffer_.end(); }

  char next();

  void next(size_t _n);

  void assertNext(char _ch);

  char peek() { return hasNext() ? *iter_ : '\0'; }

  [[nodiscard]] Iterator getIter() const { return iter_; }

  [[nodiscard]] const char *getAddr() const { return buffer_.data() + (iter_ - buffer_.begin()); }

 private:
  void readStream(FILE *_in);
};

void FileReadStream::readStream(FILE *_in) {
  char buf[65536];
  while (true) {
    size_t n = fread(buf, 1, sizeof(buf), _in);
    if (n == 0) { break; }
    buffer_.insert(buffer_.end(), buf, buf + n);
  }
}

inline char FileReadStream::next() {
  if (hasNext()) {
    char ch = *iter_;
    iter_++;
    return ch;
  }
  return '\0';
}

inline void FileReadStream::next(size_t _n) {
  for (size_t i = 0; i < _n; ++i) {
    if (hasNext()) { iter_++; }
    else { break; }
  }
}

inline void FileReadStream::assertNext(char _ch) {
  assert(peek() == _ch);
  next();
}

} // namespace neujson

#endif //NEUJSON_NEUJSON_FILE_READ_STREAM_H_
