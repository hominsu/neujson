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
#include "neujson/exception.h"
#include "neujson/file_read_stream.h"
#include "neujson/file_write_stream.h"
#include "neujson/string_write_stream.h"
#include "neujson/writer.h"
#include "neujson/pretty_writer.h"
#include "nlohmann/json.hpp"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/stringbuffer.h"

#include <benchmark/benchmark.h>

#include <sstream>
#include <fstream>

template<class ...ExtraArgs>
void BM_neujson_read_parse(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
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

    auto err = doc.parseStream(is);
    benchmark::DoNotOptimize(err);

    if (err != neujson::error::PARSE_OK) {
      _state.SkipWithError(neujson::parseErrorStr(err));
      break;
    }
  }
}

template<class ...ExtraArgs>
void BM_neujson_read_parse_write_file(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
    FILE *output = fopen("/dev/null", "w");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }
    if (output == nullptr) { exit(EXIT_FAILURE); }

    neujson::Document doc;
    neujson::FileReadStream is(input);
    fclose(input);

    auto err = doc.parseStream(is);
    benchmark::DoNotOptimize(err);

    if (err != neujson::error::PARSE_OK) {
      _state.SkipWithError(neujson::parseErrorStr(err));
      break;
    }

    char writeBuffer[65536];
    neujson::FileWriteStream os(output, writeBuffer, sizeof(writeBuffer));
    neujson::Writer<neujson::FileWriteStream> writer(os);
    doc.WriteTo(writer);
    os.flush();
    fclose(output);
  }
}

template<class ...ExtraArgs>
void BM_neujson_read_parse_write_string(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
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

    auto err = doc.parseStream(is);
    benchmark::DoNotOptimize(err);

    if (err != neujson::error::PARSE_OK) {
      _state.SkipWithError(neujson::parseErrorStr(err));
      break;
    }

    neujson::StringWriteStream os;
    neujson::Writer<neujson::StringWriteStream> writer(os);
    doc.WriteTo(writer);
    std::string_view ret = os.get();
    (void) ret;
  }
}

template<class ...ExtraArgs>
void BM_neujson_read_parse_pretty_write_file(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
    FILE *output = fopen("/dev/null", "w");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }
    if (output == nullptr) { exit(EXIT_FAILURE); }

    neujson::Document doc;
    neujson::FileReadStream is(input);
    fclose(input);

    auto err = doc.parseStream(is);
    benchmark::DoNotOptimize(err);

    if (err != neujson::error::PARSE_OK) {
      _state.SkipWithError(neujson::parseErrorStr(err));
      break;
    }

    char writeBuffer[65536];
    neujson::FileWriteStream os(output, writeBuffer, sizeof(writeBuffer));
    neujson::PrettyWriter<neujson::FileWriteStream> writer(os);
    doc.WriteTo(writer);
    os.flush();
    fclose(output);
  }
}

template<class ...ExtraArgs>
void BM_neujson_read_parse_pretty_write_string(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
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

    auto err = doc.parseStream(is);
    benchmark::DoNotOptimize(err);

    if (err != neujson::error::PARSE_OK) {
      _state.SkipWithError(neujson::parseErrorStr(err));
      break;
    }

    neujson::StringWriteStream os;
    neujson::PrettyWriter<neujson::StringWriteStream> writer(os);
    doc.WriteTo(writer);
    std::string_view ret = os.get();
    (void) ret;
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read_parse(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
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

    auto err = doc.GetParseError();
    benchmark::DoNotOptimize(err);

    if (doc.HasParseError()) {
      _state.SkipWithError(rapidjson::GetParseError_En(err));
      break;
    }
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read_parse_write_file(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
    FILE *output = fopen("/dev/null", "w");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }
    if (output == nullptr) { exit(EXIT_FAILURE); }

    char readBuffer[65536];
    rapidjson::FileReadStream is(input, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(input);

    auto err = doc.GetParseError();
    benchmark::DoNotOptimize(err);

    if (doc.HasParseError()) {
      _state.SkipWithError(rapidjson::GetParseError_En(err));
      break;
    }

    char writeBuffer[65536];
    rapidjson::FileWriteStream os(output, writeBuffer, sizeof(writeBuffer));

    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    doc.Accept(writer);

    fclose(output);
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read_parse_write_string(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
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

    auto err = doc.GetParseError();
    benchmark::DoNotOptimize(err);

    if (doc.HasParseError()) {
      _state.SkipWithError(rapidjson::GetParseError_En(err));
      break;
    }

    rapidjson::StringBuffer os;
    rapidjson::Writer<rapidjson::StringBuffer> writer(os);
    doc.Accept(writer);
    std::string_view ret = os.GetString();
    (void) ret;
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read_parse_pretty_write_file(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
#if defined(_MSC_VER)
    FILE *input;
    fopen_s(&input, _extra_args..., "r");
#else
    FILE *input = fopen(_extra_args..., "r");
    FILE *output = fopen("/dev/null", "w");
#endif
    if (input == nullptr) { exit(EXIT_FAILURE); }
    if (output == nullptr) { exit(EXIT_FAILURE); }

    char readBuffer[65536];
    rapidjson::FileReadStream is(input, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(input);

    auto err = doc.GetParseError();
    benchmark::DoNotOptimize(err);

    if (doc.HasParseError()) {
      _state.SkipWithError(rapidjson::GetParseError_En(err));
      break;
    }

    char writeBuffer[65536];
    rapidjson::FileWriteStream os(output, writeBuffer, sizeof(writeBuffer));

    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
    doc.Accept(writer);

    fclose(output);
  }
}

template<class ...ExtraArgs>
void BM_rapidjson_read_parse_pretty_write_string(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
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

    auto err = doc.GetParseError();
    benchmark::DoNotOptimize(err);

    if (doc.HasParseError()) {
      _state.SkipWithError(rapidjson::GetParseError_En(err));
      break;
    }

    rapidjson::StringBuffer os;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(os);
    doc.Accept(writer);
    std::string_view ret = os.GetString();
    (void) ret;
  }
}

template<class ...ExtraArgs>
void BM_nlohmann_read_parse(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
    ::std::ifstream ifs(_extra_args...);
    if (!ifs.is_open()) { exit(EXIT_FAILURE); }

    ::nlohmann::json j;
    benchmark::DoNotOptimize(j);
    ifs >> j;
    ifs.close();
  }
}

template<class ...ExtraArgs>
void BM_nlohmann_read_parse_write_file(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
    ::std::ifstream ifs(_extra_args...);
    if (!ifs.is_open()) { exit(EXIT_FAILURE); }
    ::std::ofstream ofs("/dev/null");
    if (!ofs.is_open()) { exit(EXIT_FAILURE); }

    ::nlohmann::json j;
    benchmark::DoNotOptimize(j);
    ifs >> j;
    ifs.close();

    ofs << j << std::flush;
    ofs.close();
  }
}

template<class ...ExtraArgs>
void BM_nlohmann_read_parse_write_string(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
    ::std::ifstream ifs(_extra_args...);
    if (!ifs.is_open()) { exit(EXIT_FAILURE); }

    ::nlohmann::json j;
    benchmark::DoNotOptimize(j);
    ifs >> j;
    ifs.close();

    ::std::stringstream ss;

    ss << j << std::flush;
    ::std::string str(ss.str());
    std::string_view ret(str);
    (void) ret;
  }
}

template<class ...ExtraArgs>
void BM_nlohmann_read_parse_pretty_write_file(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
    ::std::ifstream ifs(_extra_args...);
    if (!ifs.is_open()) { exit(EXIT_FAILURE); }
    ::std::ofstream ofs("/dev/null");
    if (!ofs.is_open()) { exit(EXIT_FAILURE); }

    ::nlohmann::json j;
    benchmark::DoNotOptimize(j);
    ifs >> j;
    ifs.close();

    ofs << ::std::setw(4) << j << std::flush;
    ofs.close();
  }
}

template<class ...ExtraArgs>
void BM_nlohmann_read_parse_pretty_write_string(benchmark::State &_state, ExtraArgs &&... _extra_args) {
  for (auto s: _state) {
    (void) s;
    ::std::ifstream ifs(_extra_args...);
    if (!ifs.is_open()) { exit(EXIT_FAILURE); }

    ::nlohmann::json j;
    benchmark::DoNotOptimize(j);
    ifs >> j;
    ifs.close();

    ::std::stringstream ss;

    ss << ::std::setw(4) << j << std::flush;
    ::std::string str(ss.str());
    std::string_view ret(str);
    (void) ret;
  }
}

// @formatter:off
BENCHMARK_CAPTURE(BM_neujson_read_parse, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse_write_file, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse_write_file, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_write_file, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse_write_string, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse_write_string, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_write_string, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond); // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse_pretty_write_file, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse_pretty_write_file, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_pretty_write_file, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse_pretty_write_string, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse_pretty_write_string, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_pretty_write_string, citm_catalog.json, "../../citm_catalog.json")->Unit(benchmark::kMillisecond);  // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse_write_file, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse_write_file, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_write_file, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse_write_string, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse_write_string, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_write_string, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond);  // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse_pretty_write_file, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse_pretty_write_file, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_pretty_write_file, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT

BENCHMARK_CAPTURE(BM_neujson_read_parse_pretty_write_string, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_nlohmann_read_parse_pretty_write_string, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT
BENCHMARK_CAPTURE(BM_rapidjson_read_parse_pretty_write_string, canada.json, "../../canada.json")->Unit(benchmark::kMillisecond); // NOLINT
// @formatter:on

BENCHMARK_MAIN();