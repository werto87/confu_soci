cmake_minimum_required(VERSION 3.27)
project(project VERSION 1.0.0
        DESCRIPTION project
        LANGUAGES CXX
        )
include(cmake/Cache.cmake)
find_package(Boost REQUIRED)
find_package(SOCI REQUIRED)
find_package(magic_enum REQUIRED)
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
target_include_directories(${TARGET_NAME_LIB} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
target_link_libraries(${TARGET_NAME_LIB} PUBLIC
        ${PROJECT_WARNINGS}
        ${PROJECT_OPTIONS}
        boost::boost
        soci::soci
        magic_enum::magic_enum
        )
OPTION(BUILD_TESTS "enable tests" OFF)
IF (BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
ENDIF (BUILD_TESTS)
