//
// Created by HominSu on 2022/3/12.
//

#include "neujson/document.h"
#include "neujson/pretty_writer.h"
#include "neujson/file_read_stream.h"
#include "example/sample.h"

#include <cstdio>

int main() {
  FILE *input;
  fopen_s(&input, "../../citm_catalog.json", "r");
  if (input == nullptr) { exit(EXIT_FAILURE); }
  neujson::FileReadStream is(input);
  fclose(input);

  neujson::Document doc;
  neujson::error::ParseError err;
  if ((err = doc.parseStream(is)) != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  neujson::FileReadStream os(stdout);
  neujson::PrettyWriter writer(os);
  writer.SetIndent(' ', 2);
  doc.writeTo(writer);

  return 0;
}

