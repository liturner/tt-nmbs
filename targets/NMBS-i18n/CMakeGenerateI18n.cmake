include("${NMBS_CONFIG_FILE}")

message(STATUS "Extracting strings with xgettext...")
execute_process(
        COMMAND "${XGETTEXT_BIN}"
        --from-code=UTF-8
        -o "${NMBS_POT_FILE}"
        -f "${NMBS_SOURCES_LIST_FILE}"
        -d nmbs
        -j
        --no-location
        --omit-header
        --package-name=nmbs
        --package-version=${NMBS_PROJECT_VERSION}
        WORKING_DIRECTORY "${NMBS_SOURCE_DIR}"
        RESULT_VARIABLE xgettext_result
)

if(NOT xgettext_result EQUAL 0)
    message(FATAL_ERROR "xgettext failed to extract strings.")
endif()


file(GLOB PO_FILES "po/*.po")
if(NOT PO_FILES)
    message(STATUS "No .po files found in po/. Skipping msgmerge.")
    return()
endif()


foreach(PO_FILE IN LISTS PO_FILES)
    message(STATUS "Updating ${PO_FILE}...")
    execute_process(
            COMMAND "${MSGMERGE_BIN}" --update --backup=none "${PO_FILE}" "nmbs.pot"
            RESULT_VARIABLE msgmerge_result
    )
    if(NOT msgmerge_result EQUAL 0)
        message(FATAL_ERROR "msgmerge failed on ${PO_FILE}.")
    endif()
endforeach()

message(STATUS "Translation source update complete.")