//
// Created by HominSu on 2022/3/12.
//

#if defined(__SSE4_2__)
#define RAPIDJSON_SSE42
#define NEUJSON_SSE42
#elif defined(__SSE2__)
#define RAPIDJSON_SSE2
#define NEUJSON_SSE2
#elif defined(__ARM_NEON)
#define RAPIDJSON_NEON
#define NEUJSON_NEON
#endif

#include "neujson/document.h"
#include "neujson/file_read_stream.h"
#include "neujson/string_write_stream.h"
#include "neujson/writer.h"
#include "neujson/pretty_writer.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"

#include "nlohmann/json.hpp"

#include <benchmark/benchmark.h>

#include <string>
#include <sstream>
#include <fstream>

template<class ...ExtraArgs>
void BM_neujson_read_parse(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }
    neujson::Document doc;
    neujson::FileReadStream is(input);
    fclose(input);

    if (doc.parseStream(is) != neujson::error::PARSE_OK) { exit(EXIT_FAILURE); }
  }
}

template<class ...ExtraArgs>
void BM_neujson_read_parse_write(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }
    neujson::Document doc;
    neujson::FileReadStream is(input);
    fclose(input);

    if (doc.parseStream(is) != neujson::error::PARSE_OK) { exit(EXIT_FAILURE); }

    neujson::StringWriteStream os;
    neujson::Writer writer(os);
    doc.WriteTo(writer);
    std::string_view ret = os.get();

    benchmark::DoNotOptimize(ret);
  }
}

template<class ...ExtraArgs>
void BM_neujson_read_parse_pretty_write(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }
    neujson::Document doc;
    neujson::FileReadStream is(input);
    fclose(input);

    if (doc.parseStream(is) != neujson::error::PARSE_OK) { exit(EXIT_FAILURE); }

    neujson::StringWriteStream os;
    neujson::PrettyWriter writer(os);
    doc.WriteTo(writer);
    std::string_view ret = os.get();

    benchmark::DoNotOptimize(ret);
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read_parse(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }

    char readBuffer[65536];
    rapidjson::FileReadStream is(input, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);

    fclose(input);
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read_parse_write(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }

    char readBuffer[65536];
    rapidjson::FileReadStream is(input, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);

    fclose(input);

    rapidjson::StringBuffer os;
    rapidjson::Writer<rapidjson::StringBuffer> writer(os);
    doc.Accept(writer);
    std::string_view ret = os.GetString();

    benchmark::DoNotOptimize(ret);
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read_parse_pretty_write(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }

    char readBuffer[65536];
    rapidjson::FileReadStream is(input, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);

    fclose(input);

    rapidjson::StringBuffer os;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(os);
    doc.Accept(writer);
    std::string_view ret = os.GetString();

    benchmark::DoNotOptimize(ret);
  }
}

template<class ...ExtraArgs>
void BM_nlohmann_read_parse(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
    ::std::ifstream ifs(_extra_args...);
    if (!ifs.is_open()) { exit(EXIT_FAILURE); }

    ::nlohmann::json j;
    ifs >> j;

    ifs.close();

    benchmark::DoNotOptimize(j);
  }
}

template<class ...ExtraArgs>
void BM_nlohmann_read_parse_write(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
    ::std::ifstream ifs(_extra_args...);
    if (!ifs.is_open()) { exit(EXIT_FAILURE); }

    ::nlohmann::json j;
    ifs >> j;

    ifs.close();

    ::std::stringstream ss;

    ss << j << std::endl;
    ::std::string str(ss.str());
    std::string_view ret(str);

    benchmark::DoNotOptimize(ret);
  }
}

template<class ...ExtraArgs>
void BM_nlohmann_read_parse_pretty_write(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
    ::std::ifstream ifs(_extra_args...);
    if (!ifs.is_open()) { exit(EXIT_FAILURE); }

    ::nlohmann::json j;
    ifs >> j;

    ifs.close();

    ::std::stringstream ss;

    ss << ::std::setw(4) << j << std::endl;
    ::std::string str(ss.str());
    std::string_view ret(str);

    benchmark::DoNotOptimize(ret);
  }
}

// @formatter:off
BENCHMARK_CAPTURE(BM_neujson_read_parse, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
//BENCHMARK_CAPTURE(BM_neujson_read_parse_write, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
//BENCHMARK_CAPTURE(BM_neujson_read_parse_pretty_write, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT

BENCHMARK_CAPTURE(BM_nlohmann_read_parse, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
//BENCHMARK_CAPTURE(BM_nlohmann_read_parse_write, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
//BENCHMARK_CAPTURE(BM_nlohmann_read_parse_pretty_write, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT

BENCHMARK_CAPTURE(BM_rapidjson_read_parse, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
//BENCHMARK_CAPTURE(BM_rapidjson_read_parse_write, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
//BENCHMARK_CAPTURE(BM_rapidjson_read_parse_pretty_write, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
//BENCHMARK_CAPTURE(BM_neujson_read_parse_write, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
//BENCHMARK_CAPTURE(BM_neujson_read_parse_pretty_write, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT

BENCHMARK_CAPTURE(BM_nlohmann_read_parse, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
//BENCHMARK_CAPTURE(BM_nlohmann_read_parse_write, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
//BENCHMARK_CAPTURE(BM_nlohmann_read_parse_pretty_write, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT

BENCHMARK_CAPTURE(BM_rapidjson_read_parse, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
//BENCHMARK_CAPTURE(BM_rapidjson_read_parse_write, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
//BENCHMARK_CAPTURE(BM_rapidjson_read_parse_pretty_write, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT
// @formatter:on

BENCHMARK_MAIN();