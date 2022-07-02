CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
if (POLICY CMP0025)
    # detect Apple's Clang
    cmake_policy(SET CMP0025 NEW)
endif ()
if (POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif ()

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

set(LIB_MAJOR_VERSION "1")
set(LIB_MINOR_VERSION "0")
set(LIB_PATCH_VERSION "3")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

if (CMAKE_VERSION VERSION_LESS 3.0)
    PROJECT(neujson CXX)
else ()
    cmake_policy(SET CMP0048 NEW)
    PROJECT(neujson VERSION "${LIB_VERSION_STRING}" LANGUAGES C CXX)
endif ()

# compile in release with debug info mode by default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# Build all binaries in a separate directory
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(NEUJSON_BUILD_EXAMPLES "Build neujson examples." ON)
option(NEUJSON_BUILD_BENCHMARK "Build neujson benchmark." OFF)
option(NEUJSON_BUILD_TESTS "Build neujson unittests." ON)
option(NEUJSON_ENABLE_INSTRUMENTATION_OPT "Build neujson with -march or -mcpu options" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

message("")
message("Operation system is ${CMAKE_SYSTEM}")
message("Current compiler: ${CMAKE_CXX_COMPILER_ID}")
message("Current compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message("Current compiler directory: ${CMAKE_CXX_COMPILER}")

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
    endif ()
endif (CCACHE_FOUND)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (NEUJSON_ENABLE_INSTRUMENTATION_OPT AND NOT CMAKE_CROSSCOMPILING)
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "powerpc" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc64le")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=native")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        endif ()
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    set(EXTRA_CXX_FLAGS -Weffc++ -Wswitch-default -Wfloat-equal -Wconversion -Wsign-conversion)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (NOT CMAKE_CROSSCOMPILING)
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "powerpc" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc64le")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=native")
        elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64" AND CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=apple-m1")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        endif ()
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-missing-field-initializers")
    set(EXTRA_CXX_FLAGS -Weffc++ -Wswitch-default -Wfloat-equal -Wconversion -Wimplicit-fallthrough)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    # Always compile with /WX
    if (CMAKE_CXX_FLAGS MATCHES "/WX-")
        string(REGEX REPLACE "/WX-" "/WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "XL")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qarch=auto")
endif ()


include_directories(${PROJECT_SOURCE_DIR}/include)

#add extra search paths for libraries and includes
SET(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")
SET(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "Directory where lib will install")


if (NEUJSON_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()

if(NEUJSON_BUILD_TESTS)
    if(MSVC11)
        # required for VS2012 due to missing support for variadic templates
        add_definitions(-D_VARIADIC_MAX=10)
    endif(MSVC11)
    add_subdirectory(third_party/gtest)
    add_subdirectory(test)
endif(NEUJSON_BUILD_TESTS)

if (NEUJSON_BUILD_BENCHMARK)
    include_directories(third_party/rapidjson/include)
    include_directories(third_party/nlohmann-json/include)
    if(NOT NEUJSON_BUILD_TESTS)
        add_subdirectory(third_party/gtest)
    endif()
    add_subdirectory(third_party/benchmark)
    add_subdirectory(bench)
endif (NEUJSON_BUILD_BENCHMARK)

# header
install(DIRECTORY include/neujson
        DESTINATION "${INCLUDE_INSTALL_DIR}"
        COMPONENT dev)

# cmake
IF (UNIX OR CYGWIN)
    SET(_CMAKE_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}")
ELSEIF (WIN32)
    SET(_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/cmake")
ENDIF ()
SET(CMAKE_INSTALL_DIR "${_CMAKE_INSTALL_DIR}" CACHE PATH "The directory cmake files are installed in")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion)
configure_file(
        "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        @ONLY)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DIR})

# pkg-config
IF (UNIX OR CYGWIN)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
            @ONLY)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
            DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
            COMPONENT pkgconfig)
ENDIF ()

# uninstall target
if (NOT TARGET uninstall)
    configure_file(
            "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Uninstall.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake"
            IMMEDIATE @ONLY)
    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake)
endif ()

message("The directory the headers are installed in: ${CMAKE_INSTALL_PREFIX}")
message("Directory where lib will install: ${LIB_INSTALL_DIR}")
message("")
