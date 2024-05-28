//
// Created by Homing So on 2022/3/9.
//

#ifndef NEUJSON_NEUJSON_STRING_READ_STREAM_H_
#define NEUJSON_NEUJSON_STRING_READ_STREAM_H_

#include <string_view>

#include "neujson.h"
#include "non_copyable.h"

namespace neujson {

class StringReadStream : public NonCopyable {
public:
  using Iterator = std::string_view::iterator;

private:
  std::string_view json_;
  Iterator iter_;

public:
  explicit StringReadStream(const std::string_view json)
      : json_(json), iter_(json_.begin()) {}

  [[nodiscard]] bool hasNext() const { return iter_ != json_.end(); }

  [[nodiscard]] char peek() const { return hasNext() ? *iter_ : '\0'; }

  char next() {
    if (hasNext()) {
      const char ch = *iter_;
      iter_++;
      return ch;
    }
    return '\0';
  }

  std::string_view next(const std::size_t n) {
    auto start = iter_;
    if (static_cast<std::size_t>(std::distance(iter_, json_.end())) >= n) {
      std::advance(iter_, n);
    }
    return {start, iter_};
  }

  void skip(const std::size_t n) {
    if (static_cast<std::size_t>(std::distance(iter_, json_.end())) >= n) {
      std::advance(iter_, n);
    }
  }

  void assertNext(const char ch) {
    (void)ch;
    NEUJSON_ASSERT(peek() == ch);
    next();
  }
};

} // namespace neujson

#endif // NEUJSON_NEUJSON_STRING_READ_STREAM_H_
