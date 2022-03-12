file(GLOB ProjectSource "*.cc")
file(GLOB ProjectHeader "*.h")

add_library(neujson STATIC ${ProjectSource} ${ProjectHeader})
install(TARGETS neujson DESTINATION lib)

set(HEADERS ${ProjectHeader})
install(FILES ${HEADERS} DESTINATION include)
