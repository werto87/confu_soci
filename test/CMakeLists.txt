add_executable(_test
        test.cxx
        testHelper.cxx
        )
find_package(Catch2)
target_link_libraries(_test
        myproject_options
        myproject_warnings
        confu_soci
        Catch2::Catch2WithMain
        )
target_include_directories(_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if (CMAKE_BINARY_DIR)
    target_compile_definitions(_test PRIVATE PATH_TO_BINARY="${CMAKE_BINARY_DIR}")
endif ()
add_test(NAME _test
        COMMAND _test -r junit)