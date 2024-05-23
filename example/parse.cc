//
// Created by Homing So on 2022/3/15.
//

#include <cstdio>

#include "neujson/file_write_stream.h"
#include "neujson/reader.h"
#include "neujson/string_read_stream.h"
#include "neujson/writer.h"

int main() {
  neujson::StringReadStream in(R"({"key":"value"})");

  char writeBuffer[65536];

  neujson::FileWriteStream out(stdout, writeBuffer);
  neujson::Writer writer(out);

  if (const auto err = neujson::Reader::Parse(in, writer);
      err != neujson::error::OK) {
    puts(neujson::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  return 0;
}
