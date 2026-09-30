file(GLOB EXAMPLE_SRC_FILES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/example/*.cc
)

foreach (_example_file ${EXAMPLE_SRC_FILES})
    get_filename_component(_example_name ${_example_file} NAME_WE)

    add_executable(${_example_name}
            ${_example_file}
    )

    target_link_libraries(${_example_name} PRIVATE
            ${PROJECT_NAME}
    )
endforeach ()