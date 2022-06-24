# Threads
find_package(Threads REQUIRED)
if (Threads_FOUND)
    message("")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_executable(bench_parse bench_parse.cc)
target_link_libraries(bench_parse Threads::Threads benchmark)
