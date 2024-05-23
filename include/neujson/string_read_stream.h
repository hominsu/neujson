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
  explicit StringReadStream(std::string_view _json)
      : json_(_json), iter_(json_.begin()) {}

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
  void next(T _n) {
    NEUJSON_ASSERT(_n >= 0);
    for (T i = 0; i < _n; ++i) {
      if (hasNext()) {
        iter_++;
      }
    }
  }

  void assertNext(const char _ch) {
    (void)_ch;
    NEUJSON_ASSERT(peek() == _ch);
    next();
  }
};

} // namespace neujson

#endif // NEUJSON_NEUJSON_STRING_READ_STREAM_H_
