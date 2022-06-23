<div id="top"></div>

<!-- PROJECT SHIELDS -->
<p align="center">
<a href="https://github.com/hominsu/neujson/graphs/contributors"><img src="https://img.shields.io/github/contributors/hominsu/neujson.svg?style=for-the-badge" alt="Contributors"></a>
<a href="https://github.com/hominsu/neujson/network/members"><img src="https://img.shields.io/github/forks/hominsu/neujson.svg?style=for-the-badge" alt="Forks"></a>
<a href="https://github.com/hominsu/neujson/stargazers"><img src="https://img.shields.io/github/stars/hominsu/neujson.svg?style=for-the-badge" alt="Stargazers"></a>
<a href="https://github.com/hominsu/neujson/issues"><img src="https://img.shields.io/github/issues/hominsu/neujson.svg?style=for-the-badge" alt="Issues"></a>
<a href="https://github.com/hominsu/neujson/blob/master/LICENSE"><img src="https://img.shields.io/github/license/hominsu/neujson.svg?style=for-the-badge" alt="License"></a>
<a href="https://github.com/hominsu/neujson/actions/workflows/docker-publish.yml"><img src="https://img.shields.io/github/workflow/status/hominsu/neujson/Docker%20Deploy?style=for-the-badge" alt="Deploy"></a>
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
    <a href="https://hominsu.github.io/neujson/"><strong>Explore the docs » (you are here)</strong></a>
    <br/>
    <br/>
    <a href="https://github.com/hominsu/neujson">View Demo</a>
    ·
    <a href="https://github.com/hominsu/neujson/issues">Report Bug</a>
    ·
    <a href="https://github.com/hominsu/neujson/issues">Request Feature</a>
  </p>
</div>

## Description

neujson is a JSON parser and generator for C++. It supports both SAX and DOM style API.

## Install

### clone the neujson repo

```bash
git clone https://github.com/hominsu/neujson.git
```

### Build and install

The following commands build and locally install neujson:

```bash
cd neujson
mkdir -p cmake/build
pushd cmake/build
cmake ../..
make -j
make install
popd
```

If you didn't have root access use `sudo` to uninstall neujson instead:

```bash
sudo make uninstall
```

#### Build with example

The example code is part of the `neujson` repo source, which you cloned as part of the steps of the previous section, just add a cmake option:

```bash
cmake -DNEUJSON_BUILD_EXAMPLES=ON ../..
```

#### Build with benchmark

To build with benchmark，use `git submodule` to fetch all the data from that third party project and check out the appropriate commit first, then add the benchmark CMake option:

```bash
git submodule update --init --recursive
pushd cmake/build
cmake -DNEUJSON_BUILD_BENCHMARK=ON ../..
...
```

## Uninstall

The following commands uninstall neujson:

```bash
pushd cmake/build
make uninstall
popd
```

If you didn't have root access use `sudo` to install neujson instead:

```bash
sudo make install
```

## Usage at a glance

This simple example parses a JSON string into a document (DOM), make a simple modification of the DOM, and finally stringify the DOM to a JSON string.

```cpp
#include "neujson/document.h"
#include "neujson/writer.h"
#include "neujson/string_write_stream.h"
#include "sample.h"

#include <cstdio>

int main() {
  // 1. Parse a JSON string into DOM.
  neujson::Document doc;
  neujson::error::ParseError err = doc.parse(kSample[0]);
  if (err != neujson::error::PARSE_OK) {
    puts(neujson::parseErrorStr(err));
    return EXIT_FAILURE;
  }

  // 2. Modify it by DOM.
  auto &s = doc[0]["Longitude"];
  s.SetDouble(s.GetDouble() + 100.0);

  // 3. Stringify the DOM
  neujson::StringWriteStream os;
  neujson::Writer writer(os);
  doc.WriteTo(writer);

  // Output
  fprintf(stdout, "%.*s", static_cast<int>(os.get().length()), os.get().data());
  return 0;
}
```

Output:

```json
[{"precision":"zip","Latitude":37.766800000000003,"Longitude":-22.395899999999997,"Address":"","City":"SAN FRANCISCO","State":"CA","Zip":"94107","Country":"US"},{"precision":"zip","Latitude":37.371991000000001,"Longitude":-122.02602,"Address":"","City":"SUNNYVALE","State":"CA","Zip":"94085","Country":"US"}]
```

## Benchmark

The benchmark is base on the google benchmark

### JSON data

|                          JSON file                           |  Size  |                         Description                          |
| :----------------------------------------------------------: | :----: | :----------------------------------------------------------: |
| `canada.json`[source](https://github.com/mloskot/json_benchmark/blob/master/data/canada.json) | 2199KB | Contour of Canada border in [GeoJSON](http://geojson.org/) format. Contains a lot of real numbers. |
| `citm_catalog.json`[source](https://github.com/RichardHightower/json-parsers-benchmark/blob/master/data/citm_catalog.json) | 1737KB | A big benchmark file with indentation used in several Java JSON parser benchmarks. |

### Sample Results

The followings are some snapshots from the results of MacBook Air (M1, 2020) with Apple clang 13.1.6

```bash
Run on (8 X 24.1208 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x8)
  L1 Instruction 128 KiB (x8)
  L2 Unified 4096 KiB (x2)
Load Average: 3.28, 2.24, 2.33
--------------------------------------------------------------------------------------------------------
Benchmark                                                              Time             CPU   Iterations
--------------------------------------------------------------------------------------------------------
BM_neujson_read_parse/citm_catalog.json                             7.10 ms         7.10 ms           74
BM_nlohmann_read_parse/citm_catalog.json                            10.6 ms         10.6 ms           66
BM_rapidjson_read_parse/citm_catalog.json                           2.96 ms         2.96 ms          238
BM_neujson_read_parse_write_file/citm_catalog.json                  14.8 ms         14.8 ms           47
BM_nlohmann_read_parse_write_file/citm_catalog.json                 12.6 ms         12.6 ms           55
BM_rapidjson_read_parse_write_file/citm_catalog.json                4.13 ms         4.13 ms          171
BM_neujson_read_parse_write_string/citm_catalog.json                8.09 ms         8.08 ms           85
BM_nlohmann_read_parse_write_string/citm_catalog.json               12.8 ms         12.8 ms           55
BM_rapidjson_read_parse_write_string/citm_catalog.json              3.85 ms         3.85 ms          180
BM_neujson_read_parse_pretty_write_file/citm_catalog.json           30.6 ms         30.6 ms           23
BM_nlohmann_read_parse_pretty_write_file/citm_catalog.json          13.3 ms         13.3 ms           53
BM_rapidjson_read_parse_pretty_write_file/citm_catalog.json         4.38 ms         4.38 ms          161
BM_neujson_read_parse_pretty_write_string/citm_catalog.json         10.2 ms         10.2 ms           68
BM_nlohmann_read_parse_pretty_write_string/citm_catalog.json        14.2 ms         14.2 ms           50
BM_rapidjson_read_parse_pretty_write_string/citm_catalog.json       4.21 ms         4.21 ms          166
BM_neujson_read_parse/canada.json                                   31.8 ms         31.8 ms           22
BM_nlohmann_read_parse/canada.json                                  38.6 ms         38.6 ms           18
BM_rapidjson_read_parse/canada.json                                 3.33 ms         3.33 ms          209
BM_neujson_read_parse_write_file/canada.json                        74.2 ms         74.2 ms            9
BM_nlohmann_read_parse_write_file/canada.json                       47.6 ms         47.6 ms           15
BM_rapidjson_read_parse_write_file/canada.json                      12.4 ms         12.4 ms           55
BM_neujson_read_parse_write_string/canada.json                      69.5 ms         69.5 ms           10
BM_nlohmann_read_parse_write_string/canada.json                     48.6 ms         48.6 ms           14
BM_rapidjson_read_parse_write_string/canada.json                    10.7 ms         10.7 ms           64
BM_neujson_read_parse_pretty_write_file/canada.json                  151 ms          151 ms            5
BM_nlohmann_read_parse_pretty_write_file/canada.json                50.8 ms         50.8 ms           14
BM_rapidjson_read_parse_pretty_write_file/canada.json               13.7 ms         13.7 ms           51
BM_neujson_read_parse_pretty_write_string/canada.json               79.1 ms         79.1 ms            9
BM_nlohmann_read_parse_pretty_write_string/canada.json              54.9 ms         54.9 ms           13
BM_rapidjson_read_parse_pretty_write_string/canada.json             12.4 ms         12.4 ms           55
```

The followings are some snapshots from the results of i5-9500 with gcc 8.5.0 (Red Hat 8.5.0-10) in CentOS-8-Stream

```bash
Run on (6 X 4157.69 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 9216 KiB (x1)
Load Average: 0.24, 0.30, 0.33
--------------------------------------------------------------------------------------------------------
Benchmark                                                              Time             CPU   Iterations
--------------------------------------------------------------------------------------------------------
BM_neujson_read_parse/citm_catalog.json                             8.50 ms         8.48 ms           75
BM_nlohmann_read_parse/citm_catalog.json                            14.5 ms         14.5 ms           48
BM_rapidjson_read_parse/citm_catalog.json                           2.40 ms         2.40 ms          290
BM_neujson_read_parse_write_file/citm_catalog.json                  11.3 ms         11.3 ms           62
BM_nlohmann_read_parse_write_file/citm_catalog.json                 17.4 ms         17.4 ms           40
BM_rapidjson_read_parse_write_file/citm_catalog.json                3.42 ms         3.41 ms          205
BM_neujson_read_parse_write_string/citm_catalog.json                10.4 ms         10.4 ms           67
BM_nlohmann_read_parse_write_string/citm_catalog.json               17.4 ms         17.4 ms           41
BM_rapidjson_read_parse_write_string/citm_catalog.json              3.38 ms         3.38 ms          208
BM_neujson_read_parse_pretty_write_file/citm_catalog.json           24.5 ms         24.5 ms           29
BM_nlohmann_read_parse_pretty_write_file/citm_catalog.json          19.0 ms         18.9 ms           37
BM_rapidjson_read_parse_pretty_write_file/citm_catalog.json         3.79 ms         3.78 ms          186
BM_neujson_read_parse_pretty_write_string/citm_catalog.json         14.7 ms         14.6 ms           48
BM_nlohmann_read_parse_pretty_write_string/citm_catalog.json        18.4 ms         18.4 ms           39
BM_rapidjson_read_parse_pretty_write_string/citm_catalog.json       3.72 ms         3.71 ms          191
BM_neujson_read_parse/canada.json                                   27.4 ms         27.4 ms           26
BM_nlohmann_read_parse/canada.json                                  41.6 ms         41.6 ms           17
BM_rapidjson_read_parse/canada.json                                 4.58 ms         4.57 ms          153
BM_neujson_read_parse_write_file/canada.json                         101 ms          101 ms            7
BM_nlohmann_read_parse_write_file/canada.json                       53.5 ms         53.4 ms           13
BM_rapidjson_read_parse_write_file/canada.json                      13.6 ms         13.6 ms           52
BM_neujson_read_parse_write_string/canada.json                       102 ms          102 ms            7
BM_nlohmann_read_parse_write_string/canada.json                     53.1 ms         53.1 ms           13
BM_rapidjson_read_parse_write_string/canada.json                    12.3 ms         12.2 ms           57
BM_neujson_read_parse_pretty_write_file/canada.json                  167 ms          167 ms            4
BM_nlohmann_read_parse_pretty_write_file/canada.json                58.7 ms         58.6 ms           12
BM_rapidjson_read_parse_pretty_write_file/canada.json               14.4 ms         14.4 ms           49
BM_neujson_read_parse_pretty_write_string/canada.json                117 ms          117 ms            6
BM_nlohmann_read_parse_pretty_write_string/canada.json              57.4 ms         57.4 ms           11
BM_rapidjson_read_parse_pretty_write_string/canada.json             13.1 ms         13.1 ms           54
```

## Reference

[RapidJSON](https://github.com/Tencent/rapidjson): A fast JSON parser/generator for C++ with both SAX/DOM style API
