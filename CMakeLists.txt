cmake_minimum_required(VERSION 3.10)

set(LIB_MAJOR_VERSION "1")
set(LIB_MINOR_VERSION "0")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

project(neujson VERSION "${LIB_VERSION_STRING}" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

message("")
message("Operation system is ${CMAKE_SYSTEM}")
message("Current compiler: ${CMAKE_CXX_COMPILER_ID}")
message("Current compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message("Current compiler directory: ${CMAKE_CXX_COMPILER}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    link_directories(/opt/homebrew/lib)
    include_directories(/opt/homebrew/include)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    link_directories(/usr/local/lib)
    include_directories(/usr/local/include)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if (MSVC)   # if msvc
        add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    endif ()
else ()
    message(FATAL_ERROR "Platform ${CMAKE_SYSTEM} is not support for this project")
endif ()


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDebug)
    message(STATUS "CMake Build Type: Debug")
    message("")
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    ADD_DEFINITIONS(-DRelease)
    message(STATUS "CMake Build Type: Release")
    message("")
endif ()


include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(include/neujson)

if (CMAKE_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()

if (CMAKE_BUILD_BENCHMARK)
    include_directories(third_party/rapidjson/include)
    include_directories(third_party/nlohmann-json/include)
    add_subdirectory(third_party/gtest)
    add_subdirectory(third_party/benchmark)
    add_subdirectory(bench)
endif ()
