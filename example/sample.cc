#include "neujson/document.h"
#include "neujson/pretty_writer.h"
#include "neujson/file_write_stream.h"
#include "example/sample.h"

#include <cstdio>

int main() {
  for (auto &sv: kSample) {
    fprintf(stdout, "\n");

    neujson::Document document;
    neujson::error::ParseError err = document.parse(sv);
    if (err != neujson::error::PARSE_OK) {
      puts(neujson::parseErrorStr(err));
      return 1;
    }

    neujson::FileWriteStream os(stdout);
    neujson::PrettyWriter writer(os);
    writer.SetIndent(' ', 2);
    document.writeTo(writer);

    fprintf(stdout, "\n");
  }
  return 0;
}
