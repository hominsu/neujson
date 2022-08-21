cmake_minimum_required(VERSION 2.8.12)

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

set(EXAMPLES
        create_json
        document
        file
        parse
        pretty
        stream_wrapper
        struct)

include_directories(".")
include_directories("${PROJECT_SOURCE_DIR}/include/")

foreach (example ${EXAMPLES})
    add_executable(${example} ${example}/${example}.cc)
endforeach()

add_custom_target(examples ALL DEPENDS ${EXAMPLES})
