# Stream

`neujson` has built-in string input/output streams and file input/output streams

- Support  `neujson::StringReadStream` and  `neujson::StringWriteStream`，for input/output `::std::string_view` object.
- Support  `neujson::FileReadStream` and  `neujson::FileWriteStream`，for input/output `FILE` object.

```cpp
#include "neujson/document.h"
#include "neujson/pretty_writer.h"
#include "neujson/file_read_stream.h"
#include "neujson/file_write_stream.h"

#include <cstdio>

int main() {
#if defined(_MSC_VER)
  FILE *input;
  fopen_s(&input, _extra_args..., "r");
#else
  FILE *input = fopen("../../citm_catalog.json", "r");
#endif
  if (input == nullptr) { exit(EXIT_FAILURE); }
  neujson::FileReadStream is(input);
  fclose(input);

  neujson::Document doc;
  neujson::error::ParseError err;
  if ((err = doc.parseStream(is)) != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  neujson::FileWriteStream os(stdout);
  neujson::PrettyWriter writer(os);
  writer.SetIndent(' ', 2);
  doc.WriteTo(writer);

  return 0;
}
```

Output:

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