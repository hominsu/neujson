# RapidJSON
find_package(RapidJSON CONFIG REQUIRED)
if (RapidJSON_FOUND)
    message("")
endif ()

# Threads
find_package(Threads REQUIRED)
if (Threads_FOUND)
    message("")
endif ()

add_executable(bench_parse bench_parse.cc)
target_link_libraries(bench_parse neujson Threads::Threads benchmark)