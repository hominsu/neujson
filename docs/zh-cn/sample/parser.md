# 解析器

Reader 是 neujson 的解析器，可以将各种输入流中的 JSON 字符串解析，并写入到一个写入流中：

```cpp
#include "neujson/file_write_stream.h"
#include "neujson/string_read_stream.h"
#include "neujson/reader.h"
#include "neujson/writer.h"

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

输出：

```json
{"key":"value"}
```

