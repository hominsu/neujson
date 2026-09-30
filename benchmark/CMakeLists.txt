file(GLOB BENCHMARK_SRC_FILES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/benchmark/*.cc
)

foreach (_benchmark_file ${BENCHMARK_SRC_FILES})
    get_filename_component(_benchmark_name ${_benchmark_file} NAME_WE)

    add_executable(${_benchmark_name} ${_benchmark_file})

    target_compile_definitions(${_benchmark_name} PRIVATE
            RESOURCES_DIR=\"${PROJECT_SOURCE_DIR}/benchmark/resources\"
    )

    target_link_libraries(${_benchmark_name} PRIVATE
            ${PROJECT_NAME}
            benchmark::benchmark
    )
endforeach ()