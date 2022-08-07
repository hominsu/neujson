# Generator

Generating the JSON simply requires a manual call to `Handler` ( `Writer` ):

```cpp
#include <cstdio>

#include "neujson/writer.h"
#include "neujson/file_write_stream.h"

int main() {
  char writeBuffer[65536];
  neujson::FileWriteStream os(stdout, writeBuffer, sizeof(writeBuffer));
  neujson::Writer<neujson::FileWriteStream> writer(os);

  writer.StartArray();
  writer.Double(::std::numeric_limits<double>::infinity());
  writer.Double(::std::numeric_limits<double>::quiet_NaN());
  writer.Bool(true);
  writer.String(R"({"happy": true, "pi": 3.141})");
  writer.Bool(true);
  writer.EndArray();

  return 0;
}
```

Output:

```json
[Infinity,NaN,true,"{\"happy\": true, \"pi\": 3.141}",true]
```

