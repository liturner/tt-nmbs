add_executable(NMBS-CONFIDENTIALITY-LABEL)

target_sources(NMBS-CONFIDENTIALITY-LABEL
        PRIVATE
        src/nmbs-confidentiality-label.cpp
)

set_target_properties(NMBS-CONFIDENTIALITY-LABEL PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME nmbs-confidentiality-label
)

target_link_libraries(NMBS-CONFIDENTIALITY-LABEL
        PRIVATE
        NMBS-LIB
)

##
##
##

add_executable(NMBS-XMP)

target_sources(NMBS-XMP
        PRIVATE
        src/nmbs-xmp.cpp
)

set_target_properties(NMBS-XMP PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME nmbs-xmp
)

target_link_libraries(NMBS-XMP
        PRIVATE
        NMBS-LIB
)
