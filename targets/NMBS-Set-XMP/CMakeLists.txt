add_executable(NMBS-Set-XMP)

target_sources(NMBS-Set-XMP
        PRIVATE
        src/nmbs-set-xmp.cpp
)

set_target_properties(NMBS-Set-XMP PROPERTIES
        OUTPUT_NAME nmbs-set-xmp
)

target_link_libraries(NMBS-Set-XMP
        PRIVATE
        NMBS-Library
)

generate_manpage(NMBS-Set-XMP "Binds ADatP-4774 Confidentiality Labels to files supporting XMP")

install(TARGETS NMBS-Set-XMP
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
