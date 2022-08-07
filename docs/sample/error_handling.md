# Error Handling

`neujson` has test built in error types

- `neujson::error::OK`

  > ok

- `neujson::error::ROOT_NOT_SINGULAR`

  > root not singular, e.g. `null x`, `0123`, `0x0`, `0x123`

- `neujson::error::BAD_VALUE`

  > bad value, e.g. `nul`, `?`, `+0`, `+1`, `.123`, `1.`, `INF`, `inf`, `NAN`, `nan`, `[1,]`, `["a", nul]`

- `neujson::error::EXPECT_VALUE`

  > expect value, e.g. `""`, `" "`

- `neujson::error::NUMBER_TOO_BIG`

  > number too big, e.g. `1e309`, `-1e309`

- `neujson::error::BAD_STRING_CHAR`

  > bad character, e.g. `ascii 0x01-0x1f`

- `neujson::error::BAD_STRING_ESCAPE`

  > bad escape, e.g. `\v`, `\'`, `\0`, `\x12`, only `\"`, `\\`, `\/`, `\b`, `\f`, `\n`, `\r`, `\t` is valid

- `neujson::error::BAD_UNICODE_HEX`

  > bad unicode hex, e.g. `\u`, `\u/000`, `uG000`, `\u 123`

- `neujson::error::BAD_UNICODE_SURROGATE`

  > bad unicode surrogate, e.g. `\uD800`, `\uDBFF`, `\uD800\uE000`

- `neujson::error::MISS_QUOTATION_MARK`

  > miss quotation mark, e.g. `"`, `"abc`

- `neujson::error::MISS_COMMA_OR_SQUARE_BRACKET`

  > miss comma or square bracket, e.g. `[1`, `[1}`, `[1 2`, `[[]`

- `neujson::error::MISS_KEY`

  > miss key, e.g. `{:1,`, `{1:1,`, `{true:1,`, `{null:1,`, `{[]:1,`, `{{}:1,`, `{"a":1,`

- `neujson::error::MISS_COLON`

  > miss colon, e.g. `{"a"}`, `{"a","b"}`

- `neujson::error::MISS_COMMA_OR_CURLY_BRACKET`

  > miss comma or curly bracket, e.g. `{"a":1`, `{"a":1]`, `{"a":1 "b"`, `{"a":{}`

- `neujson::error::USER_STOPPED`

  > user stopped parse, e.g. user-defined Handler is incomplete

`neujson` provides error code and error message conversion functions: `neujson::parseErrorStr(error::ParseError _err)`, this function outputs a `char *` with an error message

```cpp
#include <cstdio>

#include "neujson/document.h"

int main() {
  neujson::Document doc;
  auto err = doc.parse(R"(
{
    "a":1,
    "b"
}
)");

  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }
}
```

Output:

```shell
miss colon
```
