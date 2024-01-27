add_executable(_test
        main.cxx
        test.cxx
        testHelper.cxx
        )
if (CMAKE_BINARY_DIR)
    target_compile_definitions(_test PRIVATE PATH_TO_BINARY="${CMAKE_BINARY_DIR}")
endif()

find_package(Catch2 REQUIRED)
target_link_libraries(_test
        confu_soci
        Catch2::Catch2WithMain
        )
target_include_directories(_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        )

add_test(NAME _test
        COMMAND _test -r junit
        )