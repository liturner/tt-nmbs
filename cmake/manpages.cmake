include(GNUInstallDirs)

find_program(HELP2MAN_EXECUTABLE help2man)
if(HELP2MAN_EXECUTABLE)
    message(STATUS "Found help2man: ${HELP2MAN_EXECUTABLE}")
else()
    message(WARNING "help2man not found. Man pages will not be generated.")
endif()

function(generate_manpage TARGET_NAME DESCRIPTION)
    if(NOT HELP2MAN_EXECUTABLE)
        return()
    endif()

    string(TOLOWER "${TARGET_NAME}" OUTPUT_NAME)
    set(MAN_PAGE "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME}.1")

    # Hook directly into the existing binary target.
    # NO new targets are created, so CLion stays completely clean.
    add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${HELP2MAN_EXECUTABLE}
            --no-info
            --no-discard-stderr
            --name="${DESCRIPTION}"
            --output=${MAN_PAGE}
            $<TARGET_FILE:${TARGET_NAME}>
            COMMENT "Generating man page for ${TARGET_NAME} via help2man"
            VERBATIM
    )

    # Tell CMake to install the file normally
    install(FILES ${MAN_PAGE} DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endfunction()