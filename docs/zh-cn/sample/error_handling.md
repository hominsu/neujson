# 错误处理

`neujson` 内置了这几种错误类型：

- `neujson::error::OK`

  > ok

- `neujson::error::ROOT_NOT_SINGULAR`

  > root not singular，例如：`null x`、`0123`、`0x0`、`0x123`

- `neujson::error::BAD_VALUE`

  > bad value，例如：`nul`、`?`、`+0`、`+1`、`.123`、`1.`、`INF`、`inf`、`NAN`、`nan`、`[1,]`、`["a", nul]`

- `neujson::error::EXPECT_VALUE`

  > expect value，例如：`""`、`" "`

- `neujson::error::NUMBER_TOO_BIG`

  > number too big，例如：`1e309`、`-1e309`

- `neujson::error::BAD_STRING_CHAR`

  > bad character，例如：`ascii 0x01-0x1f`

- `neujson::error::BAD_STRING_ESCAPE`

  > bad escape，例如：`\v`、`\'`、`\0`、`\x12`、只有： `\"`、`\\`、`\/`、`\b`、`\f`、`\n`、`\r`、`\t` 是正确的

- `neujson::error::BAD_UNICODE_HEX`

  > bad unicode hex，例如：`\u`、`\u/000`、`uG000`、`\u 123`

- `neujson::error::BAD_UNICODE_SURROGATE`

  > bad unicode surrogate，例如：`\uD800`、`\uDBFF`、`\uD800\uE000`

- `neujson::error::MISS_QUOTATION_MARK`

  > miss quotation mark，例如：`"`、`"abc`

- `neujson::error::MISS_COMMA_OR_SQUARE_BRACKET`

  > miss comma or square bracket，例如：`[1`、`[1}`、`[1 2`、`[[]`

- `neujson::error::MISS_KEY`

  > miss key，例如：`{:1,`、`{1:1,`、`{true:1,`、`{null:1,`、`{[]:1,`、`{{}:1,`、`{"a":1,`

- `neujson::error::MISS_COLON`

  > miss colon，例如：`{"a"}`、`{"a","b"}`

- `neujson::error::MISS_COMMA_OR_CURLY_BRACKET`

  > miss comma or curly bracket，例如：`{"a":1`、`{"a":1]`、`{"a":1 "b"`、`{"a":{}`

- `neujson::error::USER_STOPPED`

  > user stopped parse，例如：用户自定义的 Handler 没有全部定义

`neujson` 提供了错误码与错误信息转换的函数：`neujson::parseErrorStr(error::ParseError _err)`，该函数会输出一个包含错误信息的 `char *`

输出：

```cpp
#include "neujson/document.h"

#include <cstdio>

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

输出:

```shell
miss colon
```
