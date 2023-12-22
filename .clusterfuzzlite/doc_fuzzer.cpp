#include "neujson/document.h"

#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  std::string fuzz_input(reinterpret_cast<const char *>(data), size);

  neujson::Document doc;
  auto err = doc.Parse(fuzz_input);
  return 0;
}
