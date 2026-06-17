add_library(NMBS-Library SHARED)

configure_file(src/version.h.in src/nmbs/version.h @ONLY)

target_sources(NMBS-Library
        PUBLIC
        FILE_SET nmbs_headers
        TYPE HEADERS
        BASE_DIRS include
        FILES
        include/nmbs/c/nmbs.h
        include/nmbs/confidentiality_label.h
        include/nmbs/constants.h
        include/nmbs/exception.h
        include/nmbs/exceptions.h
        include/nmbs/exit_code.h
        include/nmbs/nmbs.h
        PRIVATE
        src/binding.h
        src/nmbs_private.h
        src/nmbs_c.cpp
        src/nmbs.cpp
)

set_target_properties(NMBS-Library PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME nmbs
)

target_include_directories(NMBS-Library
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        PRIVATE
        src
        ${CMAKE_CURRENT_BINARY_DIR}/src
)

target_link_libraries(NMBS-Library
        PRIVATE
        LibXml2::LibXml2
        exiv2lib
)

#
# Doxygen, man pages etc.
#

if (DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "libnmbs")
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXCLUDE_SYMBOLS std::*)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/doc/index.md")
    doxygen_add_docs(doxygen ALL
            ${CMAKE_CURRENT_SOURCE_DIR}/doc
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    install(DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/html/
            DESTINATION share/doc/libnmbs-dev/html
    )
endif ()

#
# Resources, Files etc.
#

install(FILES
        resources/nl-cl.xsd
        resources/nl-mb.xsd
        resources/xmlspif.xsd
        DESTINATION share/xml/schema/libnmbs
)
