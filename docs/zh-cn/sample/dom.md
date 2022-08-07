# 文档对象模型（DOM）

文档对象模型（Document Object Model, DOM）是一种罝于内存中的 JSON 表示方式，以供查询及操作。使用方法如下：

```cpp
#include <cstdio>

#include "neujson/document.h"
#include "neujson/writer.h"
#include "neujson/string_write_stream.h"
#include "../sample.h"

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
```

输出：

```json
[{"precision":"zip","Latitude":37.766800000000003,"Longitude":-22.395899999999997,"Address":"","City":"SAN FRANCISCO","State":"CA","Zip":"94107","Country":"US"},{"precision":"zip","Latitude":37.371991000000001,"Longitude":-122.02602,"Address":"","City":"SUNNYVALE","State":"CA","Zip":"94085","Country":"US"}]
```

