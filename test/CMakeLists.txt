set(TEST_TARGET_NAME _test)
add_executable(${TEST_TARGET_NAME}
        main.cxx
        test.cxx
        testHelper.cxx
        )
if (CMAKE_BINARY_DIR)
    target_compile_definitions(${TEST_TARGET_NAME} PRIVATE PATH_TO_BINARY="${CMAKE_BINARY_DIR}")
endif()
find_package(Catch2 REQUIRED)
target_link_libraries(${TEST_TARGET_NAME}
        ${TARGET_NAME_LIB}
        Catch2::Catch2WithMain
        )
target_include_directories(${TEST_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        )

add_test(NAME ${TEST_TARGET_NAME}
        COMMAND ${TEST_TARGET_NAME} -r junit
        )