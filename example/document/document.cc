//
// Created by Homin Su on 2022/3/15.
//

#include "neujson/document.h"
#include "neujson/writer.h"
#include "neujson/string_write_stream.h"
#include "../sample.h"

#include <cstdio>

int main() {
  // 1. Parse a JSON string into DOM.
  neujson::Document doc;
  auto err = doc.parse(kSample[0]);
  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  // 2. Modify it by DOM.
  auto &s = doc[0]["Longitude"];
  s.SetDouble(s.GetDouble() + 100.0);

  // 3. Stringify the DOM
  neujson::StringWriteStream os;
  neujson::Writer<neujson::StringWriteStream> writer(os);
  doc.WriteTo(writer);

  // Output
  fprintf(stdout, "%.*s", static_cast<int>(os.get().length()), os.get().data());
  return 0;
}
