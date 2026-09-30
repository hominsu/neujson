cmake_minimum_required(VERSION 3.10)

include(cmake/Policy.cmake NO_POLICY_SCOPE)

set(LIB_MAJOR_VERSION "1")
set(LIB_MINOR_VERSION "3")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(neujson VERSION "${LIB_VERSION_STRING}" LANGUAGES C CXX)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(${PROJECT_SOURCE_DIR}/cmake/Option.cmake)

target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if (${PROJECT_NAME_UPPER}_INSTALL)
    include(${PROJECT_SOURCE_DIR}/cmake/Install.cmake)
endif ()

if (${PROJECT_NAME_UPPER}_BUILD_TESTS OR ${PROJECT_NAME_UPPER}_BUILD_BENCHMARKS)
    add_subdirectory(third_party)
endif ()

if (${PROJECT_NAME_UPPER}_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()

if (${PROJECT_NAME_UPPER}_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()

if (${PROJECT_NAME_UPPER}_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif ()
