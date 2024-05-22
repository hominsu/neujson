//
// Created by Homing So on 2022/3/12.
//

#include <cstdio>

#include "neujson/document.h"
#include "neujson/file_read_stream.h"
#include "neujson/file_write_stream.h"
#include "neujson/pretty_writer.h"

int main() {
#if defined(_MSC_VER)
  FILE *input;
  fopen_s(&input, "../../citm_catalog.json", "r");
#else
  FILE *input = fopen("../../citm_catalog.json", "r");
#endif
  if (input == nullptr) {
    exit(EXIT_FAILURE);
  }
  neujson::FileReadStream is(input);

  neujson::Document doc;
  auto err = doc.ParseStream(is);
  fclose(input);

  if (err != neujson::error::OK) {
    puts(neujson::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  char writeBuffer[65536];
  neujson::FileWriteStream os(stdout, writeBuffer);
  neujson::PrettyWriter writer(os);
  writer.SetIndent(' ', 2);
  doc.WriteTo(writer);

  return 0;
}
