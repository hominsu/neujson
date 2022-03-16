# PrettyWriter

`PrettyWriter` can add line breaks and indentation:

```cpp
#include "neujson/document.h"
#include "neujson/file_write_stream.h"
#include "neujson/pretty_writer.h"
#include "sample.h"

#include <cstdio>

int main() {
  neujson::Document doc;
  neujson::error::ParseError err = doc.parse(kSample[0]);
  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  neujson::FileWriteStream out(stdout);
  neujson::PrettyWriter pretty_writer(out);
  pretty_writer.SetIndent(' ', 2);
  doc.WriteTo(pretty_writer);

  return 0;
}
```

Output:

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