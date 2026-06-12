add_executable(NMBS-Get-XMP)

target_sources(NMBS-Get-XMP
        PRIVATE
        src/nmbs-get-xmp.cpp
)

set_target_properties(NMBS-Get-XMP PROPERTIES
        OUTPUT_NAME nmbs-get-xmp
)

target_link_libraries(NMBS-Get-XMP
        PRIVATE
        NMBS-Library
)

generate_manpage(NMBS-Get-XMP "Reads ADatP-4774 Confidentiality Labels from files supporting XMP")
