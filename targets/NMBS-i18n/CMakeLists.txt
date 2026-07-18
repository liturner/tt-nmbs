# Unfortunately this one executable is not provided by the Gettext package
find_program(GETTEXT_XGETTEXT_EXECUTABLE NAMES xgettext)
if(GETTEXT_XGETTEXT_EXECUTABLE)
    message(STATUS "xgettext found: ${GETTEXT_XGETTEXT_EXECUTABLE}")
else()
    message(FATAL_ERROR "xgettext not found")
endif()


#
# Generate i18n_sources.txt in the build dir
#
set(SOURCES_GENEX "")
foreach(tgt NMBS-Library NMBS-Nautilus)
    get_target_property(tgt_abs_dir ${tgt} SOURCE_DIR)
    file(RELATIVE_PATH tgt_rel_dir "${CMAKE_SOURCE_DIR}" "${tgt_abs_dir}")
    if(tgt_rel_dir STREQUAL "")
        set(prefix "")
    else()
        set(prefix "${tgt_rel_dir}/")
    endif()
    set(tgt_src "$<TARGET_PROPERTY:${tgt},SOURCES>")
    set(filtered_src "$<FILTER:${tgt_src},EXCLUDE,\\.(h|hpp)$>")
    string(APPEND SOURCES_GENEX "$<IF:$<BOOL:${filtered_src}>,${prefix}$<JOIN:${filtered_src},\n${prefix}>\n,>")
endforeach()
set(SOURCES_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/i18n_sources.txt")
file(GENERATE
        OUTPUT "${SOURCES_LIST_FILE}"
        CONTENT "${SOURCES_GENEX}"
)


#
# Update .pot and .po files using the i18n_sources.txt then
# Run the Target Commands, compiling the .mo files
#
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/CMakeVariables.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/CMakeVariables.cmake"
        @ONLY
)
add_custom_target(
        NMBS-i18n
        COMMAND ${CMAKE_COMMAND}
            -D NMBS_CONFIG_FILE=${CMAKE_CURRENT_BINARY_DIR}/CMakeVariables.cmake
            -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeGenerateI18n.cmake
        COMMAND ${CMAKE_COMMAND}
            -D NMBS_CONFIG_FILE=${CMAKE_CURRENT_BINARY_DIR}/CMakeVariables.cmake
            -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeCompileI18n.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Syncing and compiling translations..."
        VERBATIM
)


#
# Finally, install the built files.
#
install(
        DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/locale/"
        DESTINATION "share/locale"
)