configure_file(resources/test-has-label.jpg resources/test-has-label.jpg COPYONLY)
configure_file(resources/test-no-xmp.jpg resources/test-no-xmp.jpg COPYONLY)
configure_file(resources/test-public-unmarked.jpg resources/test-public-unmarked.jpg COPYONLY)
configure_file(resources/spif/NATO.spif resources/spif/NATO.spif COPYONLY)
configure_file(resources/spif/PUBLIC.spif resources/spif/PUBLIC.spif COPYONLY)
configure_file(resources/xml/test.1.bdo resources/xml/test.1.bdo COPYONLY)

add_executable(NMBS-Tests
        src/nmbs/binding/access_mode.tests.cpp
        src/nmbs/binding/support.tests.cpp
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
        ../NMBS-Library/include/private
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
set_property(TEST Fixtures.Reset.test-has-label.jpg PROPERTY
        FIXTURES_SETUP TestFiles
        FIXTURES_CLEANUP TestFiles
)


# The API is intended to be very expandable. There are no positional arguments. This test
# must fail, as it doesnt specify an "object type" to expect (e.g. file, string, pipe)
add_test(NAME NMBS-Verify.ErrorWithoutFileFlag COMMAND NMBS-Verify resources/test-has-label.jpg)
set_tests_properties(NMBS-Verify.ErrorWithoutFileFlag PROPERTIES
        WILL_FAIL TRUE
)


add_test(NAME NMBS-Verify.CanRead COMMAND NMBS-Verify -f resources/test-has-label.jpg)
set_tests_properties(NMBS-Verify.CanRead PROPERTIES
        FIXTURES_REQUIRED TestFiles
        PASS_REGULAR_EXPRESSION "PUBLIC UNMARKED"
        REQUIRED_FILES "resources/test-has-label.jpg"
)


add_test(NAME NMBS-Verify.CanReadAndOutputXml COMMAND NMBS-Verify -f resources/test-public-unmarked.jpg --raw)
set_tests_properties(NMBS-Verify.CanReadAndOutputXml PROPERTIES
        PASS_REGULAR_EXPRESSION ":BindingInformation.*originatorConfidentialityLabel.*PUBLIC.*UNMARKED"
        REQUIRED_FILES "resources/test-public-unmarked.jpg"
)


add_test(NAME NMBS-Verify.CanReadEmpty COMMAND NMBS-Verify --file resources/test-no-xmp.jpg)
set_tests_properties(NMBS-Verify.CanReadEmpty PROPERTIES
        PASS_REGULAR_EXPRESSION ""
        REQUIRED_FILES "resources/test-no-xmp.jpg"
)


add_test(NAME NMBS-Set.TryWrite COMMAND NMBS-Set -f resources/test-has-label.jpg -p NATO -c RESTRICTED)
set_tests_properties(NMBS-Set.TryWrite PROPERTIES
        DEPENDS NMBS-Verify.CanRead
)


add_test(NAME NMBS-Set.VerifyWrite COMMAND NMBS-Verify -f resources/test-has-label.jpg)
set_tests_properties(NMBS-Set.VerifyWrite PROPERTIES
        DEPENDS NMBS-Set.TryWrite
        FIXTURES_REQUIRED TestFiles
        PASS_REGULAR_EXPRESSION "NATO RESTRICTED"
        REQUIRED_FILES "resources/test-has-label.jpg"
)
