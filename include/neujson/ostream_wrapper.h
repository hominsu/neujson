//
// Created by Homing So on 2022/7/1.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_OSTREAM_WRAPPER_H_
#define NEUJSON_INCLUDE_NEUJSON_OSTREAM_WRAPPER_H_

#include <sstream>

#include "non_copyable.h"

namespace neujson {

template <class Stream> class OStreamWrapper : NonCopyable {
public:
  using Ch = typename Stream::char_type;

private:
  Stream &stream_;

public:
  explicit OStreamWrapper(Stream &stream) : stream_(stream) {}

  void put(Ch ch) { stream_.put(ch); }

  void puts(const char *str, std::streamsize length) {
    stream_.write(str, length);
  }

  void put_sv(std::string_view sv) const { stream_ << sv; }

  void flush() { stream_.flush(); }
};

} // namespace neujson

#endif // NEUJSON_INCLUDE_NEUJSON_OSTREAM_WRAPPER_H_
