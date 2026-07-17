add_library(NMBS-Library SHARED)

configure_file(include/configured/nmbs/version.h.in include/configured/nmbs/version.h @ONLY)

target_sources(NMBS-Library
        PUBLIC
        FILE_SET nmbs_headers
        TYPE HEADERS
        BASE_DIRS include/public
        FILES
        include/public/nmbs/binding.h
        include/public/nmbs/confidentiality_label.h
        include/public/nmbs/exit_code.h
        include/public/nmbs/expected.h
        include/public/nmbs/nmbs.h
        include/public/nmbs/nmbs_c.h
        PRIVATE
        include/private/nmbs/nmbs_private.h
        include/private/nmbs/spif.h
        src/binding/binding.cpp
        src/binding/http.cpp
        src/binding/sidecar.cpp
        src/binding/xml.cpp
        src/binding/xmp.cpp
        src/nmbs_c.cpp
        src/xml_c.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/include/private
        ${CMAKE_CURRENT_BINARY_DIR}/include/configured
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
    set(DOXYGEN_FULL_PATH_NAMES YES)
    set(DOXYGEN_STRIP_FROM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/public)
    set(DOXYGEN_STRIP_FROM_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/public)
    doxygen_add_docs(doxygen ALL
            ${CMAKE_CURRENT_SOURCE_DIR}/doc
            ${CMAKE_CURRENT_SOURCE_DIR}/include/public
    )

    install(DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/html/
            DESTINATION share/doc/libnmbs-dev/html
    )
endif ()

#
# Resources, Files etc.
#

install(DIRECTORY
        resources/xml/
        DESTINATION share/xml/nmbs
)

