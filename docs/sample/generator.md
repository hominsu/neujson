# Generator

Generating the JSON simply requires a manual call to `Handler` ( `Writer` ):

```cpp
#include "neujson/writer.h"
#include "neujson/file_write_stream.h"

int main() {
  neujson::FileWriteStream os(stdout);
  neujson::Writer writer(os);

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

