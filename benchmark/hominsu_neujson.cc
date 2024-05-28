//
// Created by Homin Su on 24-5-29.
//

#include <filesystem>
#include <fstream>
#include <string>

#include <benchmark/benchmark.h>

#include "neujson/document.h"

#include "benchmark.h"

static void BM_decode_value(benchmark::State &state,
                            const std::filesystem::path &path) {
  auto ifs = std::ifstream(path, std::ifstream::binary);
  const std::string torrent(std::istreambuf_iterator<char>{ifs},
                            std::istreambuf_iterator<char>{});

  for (auto _ : state) {
    benchmark::DoNotOptimize(neujson::Document().Parse(torrent));
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(state.iterations() * torrent.size());
}

BENCHMARK_CAPTURE(BM_decode_value, "canada", resource::canada);
BENCHMARK_CAPTURE(BM_decode_value, "citm_catalog", resource::citm_catalog);

BENCHMARK_MAIN();
