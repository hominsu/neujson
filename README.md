<div id="top"></div>
<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

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
    <a href="https://hominsu.github.io/neujson/"><strong>Explore the docs »</strong></a>
    <br/>
    <br/>
    <a href="https://github.com/hominsu/neujson">View Demo</a>
    ·
    <a href="https://github.com/hominsu/neujson/issues">Report Bug</a>
    ·
    <a href="https://github.com/hominsu/neujson/issues">Request Feature</a>
  </p>
</div>

![Alt](https://repobeats.axiom.co/api/embed/8a83cfd0471491a4d17cc58263a6cc63143a3552.svg "Repobeats analytics image")

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

If you didn't have root access use `sudo` to install neujson instead:

```bash
sudo make install
```

#### Build with example

The example code is part of the `neujson` repo source, which you cloned as part of the steps of the previous section, just add a cmake option:

```bash
cmake -DCMAKE_BUILD_EXAMPLES ../..
```

#### Build with benchmark

To build with benchmark，use `git submodule` to fetch all the data from that third party project and check out the appropriate commit first, then add the benchmark CMake option:

```bash
git submodule update --init --recursive
pushd cmake/build
cmake -DCMAKE_BUILD_BENCHMARK ../..
...
```

## Uninstall

The following commands uninstall neujson:

```bash
pushd cmake/build
make uninstall
popd
```

If you didn't have root access use `sudo` to uninstall neujson instead:

```bash
sudo make uninstall
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
Run on (8 X 24.1207 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x8)
  L1 Instruction 128 KiB (x8)
  L2 Unified 4096 KiB (x2)
Load Average: 4.43, 2.55, 2.08
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_neujson_read_parse/citm_catalog.json         6.31 ms         6.30 ms           80
BM_nlohmann_read_parse/citm_catalog.json        8.81 ms         8.81 ms           78
BM_rapidjson_read_parse/citm_catalog.json       2.37 ms         2.37 ms          295
BM_neujson_read_parse/canada.json               29.7 ms         29.7 ms           23
BM_nlohmann_read_parse/canada.json              38.1 ms         38.1 ms           18
BM_rapidjson_read_parse/canada.json             4.04 ms         4.04 ms          172
```

## Reference

[RapidJSON](https://github.com/Tencent/rapidjson): A fast JSON parser/generator for C++ with both SAX/DOM style API

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