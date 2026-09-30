file(GLOB TEST_SRC_FILES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/test/*.cc
)

foreach (_test_file ${TEST_SRC_FILES})
    get_filename_component(_test_name ${_test_file} NAME_WE)

    add_executable(${_test_name} ${_test_file})

    target_link_libraries(${_test_name} PRIVATE
            ${PROJECT_NAME}
            GTest::gtest_main
    )

    add_test(${_test_name} ${_test_name})
    set_tests_properties(${_test_name} PROPERTIES TIMEOUT 5)
endforeach ()