enable_testing()

if (WIN32 AND (NOT CYGWIN) AND (NOT MINGW))
    set(gtest_disable_pthreads ON)
    set(gtest_force_shared_crt ON)
endif()

set(TEST_LIBRARIES gtest gtest_main)

add_custom_target(tests ALL)
add_subdirectory(unit_test)