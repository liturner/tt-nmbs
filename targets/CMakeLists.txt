add_subdirectory(NMBS-LIB)
add_subdirectory(NMBS-CLI)
add_subdirectory(NMBS-Tests)

# Add this to force the cli executables to be available for GTest
add_dependencies(NMBS-Tests NMBS-XMP)

#
# Doxygen
#

if (DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN YES)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_EXCLUDE_SYMBOLS std::*)
    doxygen_add_docs(doxygen
            NMBS-LIB/include
            NMBS-LIB/src
    )

    install(DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/man/
            TYPE MAN
    )
endif ()

#
# Resources, Files etc.
#

install(FILES
        NMBS-CLI/resources/nl-cl.xsd
        NMBS-CLI/resources/nl-mb.xsd
        DESTINATION share/xml/schema
)

#
# Targets and *Targets.cmake
#

install(TARGETS NMBS-LIB NMBS-XMP
        EXPORT ${PROJECT_NAME}Targets
        FILE_SET nmbs_headers
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

#
# *Config*.cmake
#

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        COMPATIBILITY SameMinorVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
