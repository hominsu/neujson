# 输入输出流

## 内建流

`neujson` 内置了字符串输入输出流和文件输入输出流

- 支持  `neujson::StringReadStream` 及  `neujson::StringWriteStream`，使用 `::std::string_view` 对象作输入输出。
- 支持  `neujson::FileReadStream` 及  `neujson::FileWriteStream`，使用 `FILE` 对象作输入输出。

```cpp
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
  if (input == nullptr) { exit(EXIT_FAILURE); }
  neujson::FileReadStream is(input);

  neujson::Document doc;
  auto err = doc.parseStream(is);
  fclose(input);

  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  char writeBuffer[65536];
  neujson::FileWriteStream os(stdout, writeBuffer, sizeof(writeBuffer));
  neujson::PrettyWriter<neujson::FileWriteStream> writer(os);
  writer.SetIndent(' ', 2);
  doc.WriteTo(writer);

  return 0;
}
```

输出：

```json
{
  "areaNames": {
    "205705993": "Arrière-scène central",
    "205705994": "1er balcon central",
    "205705995": "2ème balcon bergerie cour",
    "205705996": "2ème balcon bergerie jardin",
    "205705998": "1er balcon bergerie jardin",
    "205705999": "1er balcon bergerie cour",
...
```

## Std 流包装类

`neujson` 提供了正式的 `std::istream` 和 `std::ostream` 包装类。但是其性能会大大低于上述的其他流。

`IStreamWrapper` 把任何继承自 `std::istream` 的类，例如：`std::istringstream`, `std::stringstream`, `std::ifstream`, `std::fstream`

`OStreamWrapper` 把任何继承自 `std::ostream` 的类，例如：`std::ostringstream`, `std::stringstream`, `std::ofstream`, `std::fstream`

```cpp
#include <cstdio>

#include <sstream>

#include "neujson/document.h"
#include "neujson/istream_wrapper.h"
#include "neujson/ostream_wrapper.h"
#include "neujson/pretty_writer.h"
#include "../sample.h"

int main() {
  std::stringstream iss;
  std::stringstream oss;

  iss << kSample[0];

  // any class derived from std::istream, ex. std::istringstream, std::stringstream, std::ifstream, std::fstream
  neujson::IStreamWrapper<std::stringstream> is(iss);

  neujson::Document doc;
  auto err = doc.parseStream(is);

  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  // any class derived from std::ostream, ex. std::ostringstream, std::stringstream, std::ofstream, std::fstream
  neujson::OStreamWrapper<std::stringstream> os(oss);
  neujson::PrettyWriter<neujson::OStreamWrapper<std::stringstream>> pretty_writer(os);
  pretty_writer.SetIndent(' ', 2);
  doc.WriteTo(pretty_writer);

  fprintf(stdout, "%.*s", static_cast<int>(oss.str().length()), oss.str().data());
  return 0;
}
```

输出:

```json
[
  {
    "precision": "zip",
    "Latitude": 37.766800000000003,
    "Longitude": -122.3959,
    "Address": "",
    "City": "SAN FRANCISCO",
    "State": "CA",
    "Zip": "94107",
    "Country": "US"
  },
  {
    "precision": "zip",
    "Latitude": 37.371991000000001,
    "Longitude": -122.02602,
    "Address": "",
    "City": "SUNNYVALE",
    "State": "CA",
    "Zip": "94085",
    "Country": "US"
  }
]
```
