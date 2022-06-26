# Threads
find_package(Threads REQUIRED)
if (Threads_FOUND)
    message("")
endif ()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox")
    endif ()
endif ()

add_executable(bench_parse bench_parse.cc)
target_link_libraries(bench_parse Threads::Threads benchmark)
