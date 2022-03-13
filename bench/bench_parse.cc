//
// Created by HominSu on 2022/3/12.
//

#define NEUJSON_NEON

#include <neujson/document.h>
#include <neujson/file_read_stream.h>
#include <neujson/string_write_stream.h>
#include <neujson/writer.h>
#include <neujson/pretty_writer.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/stringbuffer.h>

#include <benchmark/benchmark.h>

template<class ...ExtraArgs>
void BM_neujson_read(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto _: _state) {
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }
    neujson::FileReadStream is(input);
    fclose(input);

    benchmark::DoNotOptimize(is);
  }
}

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
    doc.writeTo(writer);
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
    doc.writeTo(writer);
    std::string_view ret = os.get();

    benchmark::DoNotOptimize(ret);
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read(benchmark::State &_state, ExtraArgs &&... _extra_args) {
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

    fclose(input);
    benchmark::DoNotOptimize(is);
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

BENCHMARK_CAPTURE(BM_neujson_read, simple, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_neujson_read_parse, simple, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_neujson_read_parse_write, simple, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_neujson_read_parse_pretty_write, simple, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_rapidjson_read, simple, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_rapidjson_read_parse, simple, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_write, simple, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_pretty_write, simple, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_neujson_read, many_double, "../../canada.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_neujson_read_parse, many_double, "../../canada.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_neujson_read_parse_write, many_double, "../../canada.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_neujson_read_parse_pretty_write, many_double, "../../canada.json")->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_rapidjson_read, many_double, "../../canada.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_rapidjson_read_parse, many_double, "../../canada.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_write, many_double, "../../canada.json")->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_pretty_write, many_double, "../../canada.json")->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();