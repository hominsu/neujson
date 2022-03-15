//
// Created by Homin Su on 2022/3/15.
//

#include "neujson/document.h"
#include "neujson/file_write_stream.h"
#include "neujson/pretty_writer.h"
#include "sample.h"

#include <cstdio>

int main() {
  neujson::Document doc;
  neujson::error::ParseError err = doc.parse(kSample[0]);
  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  neujson::FileWriteStream out(stdout);
  neujson::PrettyWriter pretty_writer(out);
  pretty_writer.SetIndent(' ', 2);
  doc.WriteTo(pretty_writer);

  return 0;
}

