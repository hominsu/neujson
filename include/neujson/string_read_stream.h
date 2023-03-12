//
// Created by Homin Su on 2022/3/9.
//

#ifndef NEUJSON_NEUJSON_STRING_READ_STREAM_H_
#define NEUJSON_NEUJSON_STRING_READ_STREAM_H_

#include <string_view>

#include "neujson.h"
#include "non_copyable.h"

namespace neujson {

class StringReadStream : public NonCopyable {
 public:
  using Ch = std::string_view::value_type;
  using Iterator = std::string_view::iterator;

 private:
  std::string_view json_;
  Iterator iter_;

 public:
  explicit StringReadStream(std::string_view _json) : json_(_json), iter_(json_.begin()) {}

  [[nodiscard]] bool hasNext() const { return iter_ != json_.end(); }

  Ch peek() { return hasNext() ? *iter_ : '\0'; }

  Ch next() {
    if (hasNext()) {
      Ch ch = *iter_;
      iter_++;
      return ch;
    }
    return '\0';
  }

  template<typename Tint, class = typename std::enable_if_t<std::is_integral_v<std::remove_reference_t<Tint>>>>
  void next(Tint _n) {
    NEUJSON_ASSERT(_n >= 0);
    for (Tint i = 0; i < _n; ++i) {
      if (hasNext()) { iter_++; }
    }
  }

  void assertNext(Ch _ch) {
    (void) _ch;
    NEUJSON_ASSERT(peek() == _ch);
    next();
  }
};

} // namespace neujson

#endif //NEUJSON_NEUJSON_STRING_READ_STREAM_H_
