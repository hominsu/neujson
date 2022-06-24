add_executable(document document.cc)
target_link_libraries(document)

add_executable(file file.cc)
target_link_libraries(file)

add_executable(create_json create_json.cc)
target_link_libraries(create_json)

add_executable(parse parse.cc)
target_link_libraries(parse)

add_executable(pretty pretty.cc)
target_link_libraries(pretty)

add_executable(struct struct.cc)
target_link_libraries(struct)
