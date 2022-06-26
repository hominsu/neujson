cmake_minimum_required(VERSION 2.8)

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

set(EXAMPLES
        document
        file
        create_json
        parse
        pretty
        struct)

include_directories(".")
include_directories("../include/")

foreach (example ${EXAMPLES})
    add_executable(${example} ${example}/${example}.cc)
endforeach()

add_custom_target(examples ALL DEPENDS ${EXAMPLES})
