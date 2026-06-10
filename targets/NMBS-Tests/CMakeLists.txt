configure_file(resources/test.jpg resources/test.jpg COPYONLY)

add_executable(NMBS-Tests
        src/nmbs/Basic.tests.cpp
        src/nmbs/Test.h
)

set_target_properties(NMBS-Tests PROPERTIES
        LINKER_LANGUAGE CXX
)

target_include_directories(NMBS-Tests PRIVATE
        src
        ../NMBS-LIB/src
)

target_link_libraries(NMBS-Tests
        NMBS-LIB
        GTest::gtest_main
)

gtest_discover_tests(NMBS-Tests
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)