//
// Created by Homin Su on 2022/3/9.
//

#ifndef NEUJSON_NEUJSON_STRING_READ_STREAM_H_
#define NEUJSON_NEUJSON_STRING_READ_STREAM_H_

#include "neujson.h"
#include "noncopyable.h"

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

  char next();

  template<typename Tint, class = typename std::enable_if_t<std::is_integral_v<std::remove_reference_t<Tint>>>>
  void next(Tint &&_n);

  void assertNext(char ch) {
    NEUJSON_ASSERT(peek() == ch);
    next();
  }
};

inline char StringReadStream::next() {
  if (hasNext()) {
    char ch = *iter_;
    iter_++;
    return ch;
  }
  return '\0';
}

template<typename Tint, class>
inline void StringReadStream::next(Tint &&_n) {
  NEUJSON_ASSERT(_n >= 0);
  for (Tint i = 0; i < _n; ++i) {
    if (hasNext()) { iter_++; }
  }
}

} // namespace neujson

#endif //NEUJSON_NEUJSON_STRING_READ_STREAM_H_
