include("${NMBS_CONFIG_FILE}")

# 1. Find all .po files in the source directory
file(GLOB PO_FILES "po/*.po")

foreach(PO_FILE IN LISTS PO_FILES)
    # Get just the filename (e.g., "de.po")
    get_filename_component(PO_NAME "${PO_FILE}" NAME_WE)

    # Define the output directory: <BINARY_DIR>/locale/de/LC_MESSAGES/
    set(DEST_DIR "${NMBS_CURRENT_BINARY_DIR}/locale/${PO_NAME}/LC_MESSAGES")
    file(MAKE_DIRECTORY "${DEST_DIR}")

    # Run msgfmt
    message(STATUS "${MSGFMT_BIN}: ${PO_FILE} -> ${DEST_DIR}/nmbs.mo")
    execute_process(
            COMMAND "${MSGFMT_BIN}" -o "${DEST_DIR}/nmbs.mo" "${PO_FILE}"
            RESULT_VARIABLE msgfmt_result
    )

    if(NOT msgfmt_result EQUAL 0)
        message(FATAL_ERROR "msgfmt failed for ${PO_FILE}")
    endif()
endforeach()