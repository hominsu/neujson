//
// Created by Homin Su on 2022/3/9.
//

#ifndef NEUJSON_NEUJSON_STRING_READ_STREAM_H_
#define NEUJSON_NEUJSON_STRING_READ_STREAM_H_

#include <neujson/noncopyable.h>

#include <cassert>

#include <string_view>

namespace neujson {

class StringReadStream : public noncopyable {
 public:
  using Iterator = ::std::string_view::iterator;

 private:
  ::std::string_view json_;
  Iterator iter_;

 public:
  explicit StringReadStream(::std::string_view _json) : json_(_json), iter_(json_.begin()) {}

  [[nodiscard]] bool hasNext() const { return iter_ != json_.end(); }

  char peek() { return hasNext() ? *iter_ : '\0'; }

  [[nodiscard]] Iterator getIter() const { return iter_; }

  [[nodiscard]] const char *getAddr() const { return json_.data() + (iter_ - json_.begin()); }

  char next() {
    if (hasNext()) {
      char ch = *iter_;
      iter_++;
      return ch;
    }
    return '\0';
  };

  void next(size_t _n) {
    for (size_t i = 0; i < _n; ++i) {
      if (hasNext()) {
        iter_++;
      }
    }
  }

  void assertNext(char ch) {
    assert(peek() == ch);
    next();
  }
};

} // namespace neujson

#endif //NEUJSON_NEUJSON_STRING_READ_STREAM_H_
