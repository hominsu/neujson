//
// Created by HominSu on 2022/3/12.
//

#include "neujson/document.h"
#include "neujson/pretty_writer.h"
#include "neujson/file_read_stream.h"
#include "neujson/file_write_stream.h"

#include <cstdio>

int main() {
#if defined(_MSC_VER)
  FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
  FILE *input = fopen("../../citm_catalog.json", "r");
#endif
  if (input == nullptr) { exit(EXIT_FAILURE); }
  neujson::FileReadStream is(input);
  fclose(input);

  neujson::Document doc;
  neujson::error::ParseError err;
  if ((err = doc.parseStream(is)) != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  neujson::FileWriteStream os(stdout);
  neujson::PrettyWriter writer(os);
  writer.SetIndent(' ', 2);
  doc.WriteTo(writer);

  return 0;
}

