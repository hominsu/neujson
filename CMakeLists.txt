cmake_minimum_required(VERSION 3.21)

# setting vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")

project(neujson
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# ============================== select operating system  ==============================
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
        # because opencv is use libcmt, so don't use dll
        # use the static C/CXX library for all build types; if Debug, set MultiThreadedDebug(MTd), else, set MultiThreaded(MT)
#        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        # setting charset
        add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    endif ()
else ()
    message(FATAL_ERROR "Platform ${CMAKE_SYSTEM} is not support for this project")
endif ()

# ============================== find_package ==============================
# Threads
find_package(Threads REQUIRED)
if (Threads_FOUND)
    message("")
endif ()

# gtest
find_package(GTest CONFIG REQUIRED)
if (GTest_FOUND)
    message("")
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

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(neujson)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
add_subdirectory(example)
endif()

if (NOT CMAKE_BUILD_BENCHMARK)
add_subdirectory(bench)
endif()
