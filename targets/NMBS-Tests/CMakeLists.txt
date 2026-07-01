configure_file(resources/test-has-label.jpg resources/test-has-label.jpg COPYONLY)
configure_file(resources/test-no-xmp.jpg resources/test-no-xmp.jpg COPYONLY)
configure_file(resources/test-public-unmarked.jpg resources/test-public-unmarked.jpg COPYONLY)
configure_file(resources/spif/NATO.spif resources/spif/NATO.spif COPYONLY)
configure_file(resources/spif/PUBLIC.spif resources/spif/PUBLIC.spif COPYONLY)

add_executable(NMBS-Tests
        src/nmbs/basic.tests.cpp
        src/nmbs/http.tests.cpp
        src/nmbs/nmbs_c.tests.cpp
        src/nmbs/spif.tests.cpp
        src/nmbs/xml.tests.cpp
        src/nmbs/test.h
)

set_target_properties(NMBS-Tests PROPERTIES
        LINKER_LANGUAGE CXX
)

target_include_directories(NMBS-Tests PRIVATE
        src
        ../NMBS-Library/src
)

target_link_libraries(NMBS-Tests
        NMBS-Library
        GTest::gtest_main
)

gtest_discover_tests(NMBS-Tests
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

#
#
# CTest
#
#

add_test(NAME Fixtures.Reset.test-has-label.jpg
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/test-has-label.jpg"
        "${CMAKE_CURRENT_BINARY_DIR}/resources/test-has-label.jpg"
)

add_test(NAME NMBS-Get-XMP.CanRead COMMAND NMBS-Get-XMP resources/test-has-label.jpg)
add_test(NAME NMBS-Set-XMP.TryWrite COMMAND NMBS-Set-XMP resources/test-has-label.jpg NATO RESTRICTED)
add_test(NAME NMBS-Set-XMP.VerifyWrite COMMAND NMBS-Get-XMP resources/test-has-label.jpg)

set_property(TEST Fixtures.Reset.test-has-label.jpg PROPERTY
        FIXTURES_SETUP TestFiles
        FIXTURES_CLEANUP TestFiles
)
set_property(TEST NMBS-Set-XMP.TryWrite PROPERTY DEPENDS NMBS-Get-XMP.CanRead)
set_property(TEST NMBS-Set-XMP.VerifyWrite PROPERTY DEPENDS NMBS-Set-XMP.TryWrite)

set_tests_properties(NMBS-Get-XMP.CanRead PROPERTIES
        FIXTURES_REQUIRED TestFiles
        PASS_REGULAR_EXPRESSION "PUBLIC UNMARKED"
        REQUIRED_FILES "resources/test-has-label.jpg"
)

set_tests_properties(NMBS-Set-XMP.VerifyWrite PROPERTIES
        FIXTURES_REQUIRED TestFiles
        PASS_REGULAR_EXPRESSION "NATO RESTRICTED"
        REQUIRED_FILES "resources/test-has-label.jpg"
)
