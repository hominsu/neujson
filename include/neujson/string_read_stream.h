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

  template <typename T>
    requires std::is_integral_v<T>
  void next(T n) {
    NEUJSON_ASSERT(n >= 0);
    for (T i = 0; i < n; ++i) {
      if (hasNext()) {
        iter_++;
      }
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
