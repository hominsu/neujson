//
// Created by Homin Su on 2022/3/15.
//

#include "neujson/reader.h"
#include "neujson/writer.h"
#include "neujson/string_read_stream.h"
#include "neujson/file_write_stream.h"

#include <cstdio>

int main() {
  neujson::StringReadStream in(R"({"key":"value"})");
  neujson::FileWriteStream out(stdout);
  neujson::Writer writer(out);

  auto err = neujson::Reader::parse(in, writer);
  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  return 0;
}

