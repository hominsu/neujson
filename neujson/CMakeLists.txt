file(GLOB ProjectSource "*.cc")
file(GLOB ProjectHeader
        "internal/*.h"
        "*.h")

add_library(neujson STATIC ${ProjectSource} ${ProjectHeader})
install(TARGETS neujson
        EXPORT ${PROJECT_NAME}Targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(HEADERS ${ProjectHeader})
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${PROJECT_SOURCE_DIR}/CMakeModules/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME})

configure_file(
        "${PROJECT_SOURCE_DIR}/CMakeModules/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_SOURCE_DIR}/CMakeModules/${PROJECT_NAME}Config.cmake"
        @ONLY)
install(FILES
        "${PROJECT_SOURCE_DIR}/CMakeModules/${PROJECT_NAME}Config.cmake"
        "${PROJECT_SOURCE_DIR}/CMakeModules/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})

# uninstall target
if (NOT TARGET uninstall)
    configure_file(
            "${PROJECT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
            "${PROJECT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake"
            IMMEDIATE @ONLY)
    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif ()
