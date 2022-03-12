add_executable(sample sample.cc)
target_link_libraries(sample neujson)

add_executable(file file.cc)
target_link_libraries(file neujson)