//
// Created by Homing So on 2022/7/2.
//

#include <cstdio>

#include <sstream>

#include "neujson/document.h"
#include "neujson/istream_wrapper.h"
#include "neujson/ostream_wrapper.h"
#include "neujson/pretty_writer.h"
#include "sample.h"

int main() {
  std::stringstream iss;
  std::stringstream oss;

  iss << kSample[0];

  // any class derived from std::istream, ex. std::istringstream, std::stringstream, std::ifstream, std::fstream
  neujson::IStreamWrapper is(iss);

  neujson::Document doc;
  auto err = doc.ParseStream(is);

  if (err != neujson::error::OK) {
    puts(neujson::ParseErrorStr(err));
    return EXIT_FAILURE;
  }

  // any class derived from std::ostream, ex. std::ostringstream, std::stringstream, std::ofstream, std::fstream
  neujson::OStreamWrapper os(oss);
  neujson::PrettyWriter pretty_writer(os);
  pretty_writer.SetIndent(' ', 2);
  doc.WriteTo(pretty_writer);

  fprintf(stdout, "%.*s", static_cast<int>(oss.str().length()), oss.str().data());
  return 0;
}