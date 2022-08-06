//
// Created by HominSu on 2022/7/1.
//

#ifndef NEUJSON_INCLUDE_NEUJSON_OSTREAM_WRAPPER_H_
#define NEUJSON_INCLUDE_NEUJSON_OSTREAM_WRAPPER_H_

#include <iosfwd>
#include <sstream>

#include "noncopyable.h"

namespace neujson {

template<class Stream>
class OStreamWrapper : noncopyable {
 public:
  using Ch = typename Stream::char_type;

 private:
  Stream &stream_;

 public:
  explicit OStreamWrapper(Stream &_stream) : stream_(_stream) {}

  void put(Ch _ch) {
    stream_.put(_ch);
  }

  void puts(const char *_str, ::std::streamsize _length) {
    stream_.write(_str, _length);
  }

  void put_sv(::std::string_view _sv) {
    stream_ << _sv;
  }

  void flush() {
    stream_.flush();
  }

};

} // namespace neujson

#endif //NEUJSON_INCLUDE_NEUJSON_OSTREAM_WRAPPER_H_
