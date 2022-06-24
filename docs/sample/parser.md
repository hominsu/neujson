# Parser

Reader is a parser for neujson that parses JSON strings from various input streams and writes them to a write stream:

```cpp
#include "neujson/reader.h"
#include "neujson/writer.h"
#include "neujson/string_read_stream.h"
#include "neujson/file_write_stream.h"

#include <cstdio>

int main() {
  neujson::StringReadStream in(R"({"key":"value"})");
  char writeBuffer[65536];
  neujson::FileWriteStream out(stdout, writeBuffer, sizeof(writeBuffer));
  neujson::Writer<neujson::FileWriteStream> writer(out);

  auto err = neujson::Reader::parse(in, writer);
  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  return 0;
}
```

Output:

```json
{"key":"value"}
```

