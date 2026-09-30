set_property(DIRECTORY APPEND PROPERTY COMPILE_OPTIONS -w)

set(gtest_build_tests OFF CACHE BOOL "" FORCE)
set(gtest_build_samples OFF CACHE BOOL "" FORCE)
# Use shared (DLL) run-time lib even when Google Test is built as static lib.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest EXCLUDE_FROM_ALL)