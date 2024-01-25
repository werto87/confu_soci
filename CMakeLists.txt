cmake_minimum_required(VERSION 3.17)
project(project VERSION 1.0.0
        DESCRIPTION project
        LANGUAGES CXX
        )
find_package(Boost REQUIRED)
find_package(magic_enum REQUIRED)
find_package(SOCI REQUIRED)
include(cmake/Cache.cmake)
set(PROJECT_WARNINGS project_warning)
add_library(${PROJECT_WARNINGS} INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_WARNINGS})
set(PROJECT_OPTIONS project_options)
add_library(${PROJECT_OPTIONS} INTERFACE)
target_compile_features(${PROJECT_OPTIONS} INTERFACE cxx_std_20)
include(cmake/StaticAnalyzers.cmake)
include(cmake/Sanitizers.cmake)
enable_sanitizers(${PROJECT_OPTIONS})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TARGET_NAME_LIB confu_soci)
add_library(${TARGET_NAME_LIB})
add_subdirectory(confu_soci)
####### create executable
target_link_libraries(${TARGET_NAME_LIB} PUBLIC
        ${PROJECT_WARNINGS}
        ${PROJECT_OPTIONS}
        Boost::headers
        soci::soci
        magic_enum::magic_enum
        )
OPTION(BUILD_TESTS "enable tests" OFF)
IF (BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
ENDIF (BUILD_TESTS)
