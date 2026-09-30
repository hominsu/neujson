cmake_minimum_required(VERSION 3.10)

include(cmake/Policy.cmake NO_POLICY_SCOPE)

set(LIB_MAJOR_VERSION "1")
set(LIB_MINOR_VERSION "3")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(neujson VERSION "${LIB_VERSION_STRING}" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

include(${PROJECT_SOURCE_DIR}/cmake/Option.cmake)

target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if (${PROJECT_NAME_UPPER}_BUILD_EXAMPLES OR ${PROJECT_NAME_UPPER}_BUILD_BENCHMARKS OR ${PROJECT_NAME_UPPER}_BUILD_TESTS)
    # compile in release by default
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    endif ()

    if (${PROJECT_NAME_UPPER}_BUILD_TESTS OR ${PROJECT_NAME_UPPER}_BUILD_BENCHMARKS)
        add_subdirectory(third_party)
    endif ()

    if (${PROJECT_NAME_UPPER}_BUILD_TESTS)
        enable_testing()
        add_subdirectory(test)
    endif ()

    add_subdirectory(example)
endif ()

# add extra search paths for libraries and includes
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "Directory where lib will install")

# header
install(DIRECTORY include/${PROJECT_NAME}
        DESTINATION "${INCLUDE_INSTALL_DIR}"
        COMPONENT dev)

# cmake
if (UNIX OR CYGWIN)
    set(_CMAKE_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}")
elseif (WIN32)
    set(_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/cmake")
endif ()
set(CMAKE_INSTALL_DIR "${_CMAKE_INSTALL_DIR}" CACHE PATH "The directory cmake files are installed in")

export(PACKAGE ${PROJECT_NAME})

set(CONFIG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(${PROJECT_NAME}_INCLUDE_DIR "\${${PROJECT_NAME}_SOURCE_DIR}/include")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        @ONLY)
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}ConfigVersion.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        @ONLY)

set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME})
file(RELATIVE_PATH REL_INCLUDE_DIR
        "${CMAKECONFIG_INSTALL_DIR}"
        "${CMAKE_INSTALL_PREFIX}/include")
set(${PROJECT_NAME}_INCLUDE_DIR "\${${PROJECT_NAME}_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONFIG_SOURCE_DIR)
set(CONFIG_DIR)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

if (CMAKE_INSTALL_DIR)
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            DESTINATION "${CMAKE_INSTALL_DIR}"
            COMPONENT dev)
endif ()

# pkg-config
if (UNIX OR CYGWIN)
    configure_file(${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
            @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
            DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
            COMPONENT pkgconfig)
endif ()

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
