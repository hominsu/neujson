# benchmark
find_package(benchmark CONFIG REQUIRED)
if (benchmark_FOUND)
    message("")
endif ()

# RapidJSON
find_package(RapidJSON CONFIG REQUIRED)
if (RapidJSON_FOUND)
    message("")
endif ()

add_executable(bench_parse bench_parse.cc)
target_link_libraries(bench_parse neujson rapidjson Threads::Threads benchmark::benchmark benchmark::benchmark_main)