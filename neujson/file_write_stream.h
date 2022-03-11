//
// Created by HominSu on 2022/3/11.
//

#ifndef NEUJSON_NEUJSON_FILE_WRITE_STREAM_H_
#define NEUJSON_NEUJSON_FILE_WRITE_STREAM_H_

#include "noncopyable.h"

#include <cstdio>

#include <string_view>

namespace neujson {

class FileWriteStream : public noncopyable {
 private:
  FILE *out_;

 public:
  explicit FileWriteStream(FILE *_out) : out_(_out) {}

  void put(char _ch) {
    putc(_ch, out_);
  }

  void put(const char *_str) {
    fputs(_str, out_);
  }

  void put(::std::string_view _sv) {
    fprintf(out_, "%.*s", static_cast<int>(_sv.length()), _sv.data());
  }

  void flush() {
    fflush(out_);
  }

};

} // namespace neujson

#endif //NEUJSON_NEUJSON_FILE_WRITE_STREAM_H_
