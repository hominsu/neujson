include_guard(GLOBAL)

if (POLICY CMP0025) # detect Apple's Clang
    cmake_policy(SET CMP0025 NEW)
endif ()

if (POLICY CMP0091) # select the MSVC runtime through target properties
    cmake_policy(SET CMP0091 NEW)
endif ()
