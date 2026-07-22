add_library(NMBS-Nautilus MODULE)

target_sources(NMBS-Nautilus
        PRIVATE
        src/nmbs-nautilus.c
)

set_target_properties(NMBS-Nautilus PROPERTIES
        LINKER_LANGUAGE C
        C_STANDARD 23
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
        OUTPUT_NAME nmbs-nautilus
)

# Include directories from PkgConfig
target_include_directories(NMBS-Nautilus PRIVATE
        ${GLIB_INCLUDE_DIRS}
        ${NAUTILUS_INCLUDE_DIRS}
)

# Compiler options (fPIC is automatically handled by CMake for MODULE/SHARED libraries)
target_compile_options(NMBS-Nautilus PRIVATE
        -Wall
        -Wextra
        ${GLIB_CFLAGS_OTHER}
        ${NAUTILUS_CFLAGS_OTHER}
)

target_link_libraries(NMBS-Nautilus
        PRIVATE
        ${GLIB_LIBRARIES}
        ${NAUTILUS_LIBRARIES}
        NMBS-Library
)

# Validate the GSettings Schema file
pkg_get_variable(GLIB_COMPILE_SCHEMAS gio-2.0 glib_compile_schemas)
add_custom_command(
        TARGET NMBS-Nautilus PRE_BUILD
        COMMAND ${GLIB_COMPILE_SCHEMAS} --dry-run --strict ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Validating GSettings schemas..."
)

install(TARGETS NMBS-Nautilus
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/nautilus/extensions-4/
)

install(FILES src/org.gnome.nautilus-nmbs.gschema.xml
        DESTINATION share/glib-2.0/schemas/
)
