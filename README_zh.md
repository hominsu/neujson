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
cmake -DCMAKE_BUILD_EXAMPLES ../..
```

#### 同时构建基准测试

构建基准测试，需要使用 `git submodule` 拉取第三方依赖，并检出到相应的分支，然后添加基准测试的 `cmake` 选项：

```bash
git submodule update --init --recursive
pushd cmake/build
cmake -DCMAKE_BUILD_BENCHMARK ../..
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
sudo make install
```

## 用法一目了然

此简单例子解析一个 JSON 字符串至一个 document (DOM)，对 DOM 作出简单修改，最终把 DOM 转换（stringify）至 JSON 字符串。

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

## 参考

[RapidJSON](https://github.com/Tencent/rapidjson): A fast JSON parser/generator for C++ with both SAX/DOM style API
