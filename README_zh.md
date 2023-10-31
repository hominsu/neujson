<div id="top"></div>

<!-- PROJECT SHIELDS -->
<p align="center">
<a href="https://github.com/hominsu/neujson/graphs/contributors"><img src="https://img.shields.io/github/contributors/hominsu/neujson.svg?style=for-the-badge" alt="Contributors"></a>
<a href="https://github.com/hominsu/neujson/network/members"><img src="https://img.shields.io/github/forks/hominsu/neujson.svg?style=for-the-badge" alt="Forks"></a>
<a href="https://github.com/hominsu/neujson/stargazers"><img src="https://img.shields.io/github/stars/hominsu/neujson.svg?style=for-the-badge" alt="Stargazers"></a>
<a href="https://github.com/hominsu/neujson/issues"><img src="https://img.shields.io/github/issues/hominsu/neujson.svg?style=for-the-badge" alt="Issues"></a>
<a href="https://github.com/hominsu/neujson/blob/master/LICENSE"><img src="https://img.shields.io/github/license/hominsu/neujson.svg?style=for-the-badge" alt="License"></a>
<a href="https://github.com/hominsu/neujson/actions/workflows/docker-publish.yml"><img src="https://img.shields.io/github/actions/workflow/status/hominsu/neujson/docker-publish.yml?branch=main&style=for-the-badge" alt="Deploy"></a>
</p>


<!-- PROJECT LOGO -->
<br/>
<div align="center">
<!--   <a href="https://github.com/hominsu/neujson">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a> -->

<h3 align="center">neujson</h3>

  <p align="center">
    JSON parser/generator in C++17
    <br/>
    <a href="https://me.hauhau.cn/zh/projects/neujson/"><strong>Explore the docs »</strong></a>
    <br/>
    <br/>
    <a href="https://github.com/hominsu/neujson">View Demo</a>
    ·
    <a href="https://github.com/hominsu/neujson/issues">Report Bug</a>
    ·
    <a href="https://github.com/hominsu/neujson/issues">Request Feature</a>
  </p>
</div>

Translations: [English](README.md) | [简体中文](README_zh.md)

## 简介

neujson 是一个 C++ 的 JSON 解析器及生成器。它同时支持 SAX 和 DOM 风格的 API。

## 安装

### 克隆 neujson 的源代码

```bash
git clone https://github.com/hominsu/neujson.git
```

### 编译和安装

下面的命令将在本地构建和安装 neujson：

```bash
cd neujson
mkdir -p cmake/build
pushd cmake/build
cmake ../..
make -j
make install
popd
```

如果你没有安装目录的权限，请使用 `sudo`：

```bash
sudo make install
```

#### 同时构建示例程序

示例的源代码已经包含在 `neujson` 的源代码中，你已经在上一步中克隆了源代码，只需要额外添加一个的 `cmake` 选项：

```bash
cmake -DNEUJSON_BUILD_EXAMPLES=ON ../..
```

#### 同时构建基准测试

构建基准测试，需要使用 `git submodule` 拉取第三方依赖，并检出到相应的分支，然后添加基准测试的 `cmake` 选项：

```bash
git submodule update --init --recursive
pushd cmake/build
cmake -DNEUJSON_BUILD_BENCHMARK=ON ../..
...
```

## 卸载

使用一下命令可以卸载 neujson：

```bash
pushd cmake/build
make uninstall
popd
```

如果你没有安装目录的权限，请使用 `sudo`：

```bash
sudo make uninstall
```

## 用法一目了然

此简单例子解析一个 JSON 字符串至一个 document (DOM)，对 DOM 作出简单修改，最终把 DOM 转换（stringify）至 JSON 字符串。

```cpp
#include <cstdio>

#include "neujson/document.h"
#include "neujson/writer.h"
#include "neujson/string_write_stream.h"
#include "../sample.h"

int main() {
  // 1. Parse a JSON string into DOM.
  neujson::Document doc;
  auto err = doc.Parse(kSample[0]);
  if (err != neujson::error::OK) {
    puts(neujson::ParseErrorStr(err));
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

输出:

```json
[{"precision":"zip","Latitude":37.766800000000003,"Longitude":-22.395899999999997,"Address":"","City":"SAN FRANCISCO","State":"CA","Zip":"94107","Country":"US"},{"precision":"zip","Latitude":37.371991000000001,"Longitude":-122.02602,"Address":"","City":"SUNNYVALE","State":"CA","Zip":"94085","Country":"US"}]
```

## 基准性能测试

基准性能测试基于 google benchmark 进行

### JSON data

|                          JSON file                           |  Size  |                         Description                          |
| :----------------------------------------------------------: | :----: | :----------------------------------------------------------: |
| `canada.json`[source](https://github.com/mloskot/json_benchmark/blob/master/data/canada.json) | 2199KB | 加拿大边境轮廓，格式为: [GeoJSON](http://geojson.org/)；包含大量的实数。 |
| `citm_catalog.json`[source](https://github.com/RichardHightower/json-parsers-benchmark/blob/master/data/citm_catalog.json) | 1737KB | 一个带有缩进的大型基准测试文件，用于多个Java JSON解析器基准测试。 |

### Sample Results

下面是使用 MacBook Air (M1, 2020) 和 Apple clang 13.1.6 得到的结果

```bash
Run on (8 X 24.1212 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x8)
  L1 Instruction 128 KiB (x8)
  L2 Unified 4096 KiB (x2)
Load Average: 2.04, 1.78, 1.74
--------------------------------------------------------------------------------------------------------
Benchmark                                                              Time             CPU   Iterations
--------------------------------------------------------------------------------------------------------
BM_neujson_read_parse/citm_catalog.json                             7.04 ms         7.04 ms           72
BM_nlohmann_read_parse/citm_catalog.json                            10.6 ms         10.6 ms           66
BM_rapidjson_read_parse/citm_catalog.json                           2.96 ms         2.96 ms          236
BM_neujson_read_parse_write_file/citm_catalog.json                  7.92 ms         7.92 ms           88
BM_nlohmann_read_parse_write_file/citm_catalog.json                 12.5 ms         12.5 ms           56
BM_rapidjson_read_parse_write_file/citm_catalog.json                4.10 ms         4.10 ms          170
BM_neujson_read_parse_write_string/citm_catalog.json                8.03 ms         8.03 ms           87
BM_nlohmann_read_parse_write_string/citm_catalog.json               12.7 ms         12.7 ms           55
BM_rapidjson_read_parse_write_string/citm_catalog.json              3.90 ms         3.90 ms          180
BM_neujson_read_parse_pretty_write_file/citm_catalog.json           8.84 ms         8.84 ms           79
BM_nlohmann_read_parse_pretty_write_file/citm_catalog.json          13.3 ms         13.3 ms           53
BM_rapidjson_read_parse_pretty_write_file/citm_catalog.json         4.56 ms         4.55 ms          154
BM_neujson_read_parse_pretty_write_string/citm_catalog.json         9.44 ms         9.44 ms           72
BM_nlohmann_read_parse_pretty_write_string/citm_catalog.json        14.2 ms         14.2 ms           50
BM_rapidjson_read_parse_pretty_write_string/citm_catalog.json       4.19 ms         4.19 ms          167
BM_neujson_read_parse/canada.json                                   31.6 ms         31.6 ms           22
BM_nlohmann_read_parse/canada.json                                  39.1 ms         39.1 ms           18
BM_rapidjson_read_parse/canada.json                                 3.38 ms         3.38 ms          207
BM_neujson_read_parse_write_file/canada.json                        68.2 ms         68.2 ms           10
BM_nlohmann_read_parse_write_file/canada.json                       47.6 ms         47.6 ms           15
BM_rapidjson_read_parse_write_file/canada.json                      12.5 ms         12.5 ms           55
BM_neujson_read_parse_write_string/canada.json                      69.4 ms         69.4 ms           10
BM_nlohmann_read_parse_write_string/canada.json                     48.5 ms         48.5 ms           14
BM_rapidjson_read_parse_write_string/canada.json                    10.7 ms         10.7 ms           63
BM_neujson_read_parse_pretty_write_file/canada.json                 72.3 ms         72.3 ms           10
BM_nlohmann_read_parse_pretty_write_file/canada.json                51.2 ms         51.2 ms           14
BM_rapidjson_read_parse_pretty_write_file/canada.json               13.7 ms         13.7 ms           51
BM_neujson_read_parse_pretty_write_string/canada.json               75.9 ms         75.9 ms            9
BM_nlohmann_read_parse_pretty_write_string/canada.json              55.0 ms         55.0 ms           13
BM_rapidjson_read_parse_pretty_write_string/canada.json             12.4 ms         12.4 ms           56
```

下面是使用 i5-9500 以及 CentOS-8-Stream 和 gcc 8.5.0 (Red Hat 8.5.0-10) 得到的结果

```bash
Run on (6 X 4166.48 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 9216 KiB (x1)
Load Average: 0.80, 0.52, 0.45
--------------------------------------------------------------------------------------------------------
Benchmark                                                              Time             CPU   Iterations
--------------------------------------------------------------------------------------------------------
BM_neujson_read_parse/citm_catalog.json                             8.59 ms         8.58 ms           74
BM_nlohmann_read_parse/citm_catalog.json                            14.7 ms         14.6 ms           48
BM_rapidjson_read_parse/citm_catalog.json                           2.38 ms         2.37 ms          293
BM_neujson_read_parse_write_file/citm_catalog.json                  10.1 ms         10.1 ms           70
BM_nlohmann_read_parse_write_file/citm_catalog.json                 17.5 ms         17.5 ms           40
BM_rapidjson_read_parse_write_file/citm_catalog.json                3.39 ms         3.39 ms          206
BM_neujson_read_parse_write_string/citm_catalog.json                10.9 ms         10.9 ms           65
BM_nlohmann_read_parse_write_string/citm_catalog.json               17.5 ms         17.5 ms           40
BM_rapidjson_read_parse_write_string/citm_catalog.json              3.20 ms         3.19 ms          218
BM_neujson_read_parse_pretty_write_file/citm_catalog.json           11.3 ms         11.3 ms           60
BM_nlohmann_read_parse_pretty_write_file/citm_catalog.json          18.8 ms         18.7 ms           38
BM_rapidjson_read_parse_pretty_write_file/citm_catalog.json         3.70 ms         3.69 ms          189
BM_neujson_read_parse_pretty_write_string/citm_catalog.json         14.5 ms         14.5 ms           49
BM_nlohmann_read_parse_pretty_write_string/citm_catalog.json        18.5 ms         18.5 ms           38
BM_rapidjson_read_parse_pretty_write_string/citm_catalog.json       3.57 ms         3.57 ms          196
BM_neujson_read_parse/canada.json                                   27.8 ms         27.7 ms           25
BM_nlohmann_read_parse/canada.json                                  41.8 ms         41.8 ms           17
BM_rapidjson_read_parse/canada.json                                 4.59 ms         4.58 ms          152
BM_neujson_read_parse_write_file/canada.json                         100 ms          100 ms            7
BM_nlohmann_read_parse_write_file/canada.json                       53.5 ms         53.4 ms           13
BM_rapidjson_read_parse_write_file/canada.json                      13.6 ms         13.6 ms           51
BM_neujson_read_parse_write_string/canada.json                       106 ms          106 ms            7
BM_nlohmann_read_parse_write_string/canada.json                     53.3 ms         53.3 ms           13
BM_rapidjson_read_parse_write_string/canada.json                    11.9 ms         11.9 ms           58
BM_neujson_read_parse_pretty_write_file/canada.json                  106 ms          106 ms            7
BM_nlohmann_read_parse_pretty_write_file/canada.json                58.6 ms         58.6 ms           12
BM_rapidjson_read_parse_pretty_write_file/canada.json               14.4 ms         14.4 ms           49
BM_neujson_read_parse_pretty_write_string/canada.json                119 ms          119 ms            6
BM_nlohmann_read_parse_pretty_write_string/canada.json              64.9 ms         64.8 ms           11
BM_rapidjson_read_parse_pretty_write_string/canada.json             12.8 ms         12.8 ms           54
```

## 参考

[RapidJSON](https://github.com/Tencent/rapidjson): A fast JSON parser/generator for C++ with both SAX/DOM style API

## 鸣谢

![JetBrains Logo (Main) logo](https://resources.jetbrains.com/storage/products/company/brand/logos/jb_beam.svg)

特别感谢 [JetBrains](https://www.jetbrains.com/) 为本开源项目提供免费的 `All Products Pack` 授权

## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="http://www.hauhau.cn"><img src="https://avatars.githubusercontent.com/u/14991225?v=4?s=100" width="100px;" alt=""/><br /><sub><b>HominSu</b></sub></a><br /><a href="https://github.com/hominsu/neujson/commits?author=hominsu" title="Code">💻</a> <a href="https://github.com/hominsu/neujson/commits?author=hominsu" title="Documentation">📖</a> <a href="#design-hominsu" title="Design">🎨</a> <a href="#example-hominsu" title="Examples">💡</a> <a href="#infra-hominsu" title="Infrastructure (Hosting, Build-Tools, etc)">🚇</a> <a href="#platform-hominsu" title="Packaging/porting to new platform">📦</a> <a href="https://github.com/hominsu/neujson/commits?author=hominsu" title="Tests">⚠️</a></td>
  </tr>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
