configure_file(resources/test-has-label.jpg resources/test-has-label.jpg COPYONLY)
configure_file(resources/test-no-xmp.jpg resources/test-no-xmp.jpg COPYONLY)

add_executable(NMBS-Tests
        src/nmbs/Basic.tests.cpp
        src/nmbs/Test.h
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
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)