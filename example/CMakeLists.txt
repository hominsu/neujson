add_executable(document document.cc)
target_link_libraries(document neujson)

add_executable(file file.cc)
target_link_libraries(file neujson)

add_executable(create_json create_json.cc)
target_link_libraries(create_json neujson)

add_executable(parse parse.cc)
target_link_libraries(parse neujson)

add_executable(pretty pretty.cc)
target_link_libraries(pretty neujson)

add_executable(struct struct.cc)
target_link_libraries(struct neujson)
