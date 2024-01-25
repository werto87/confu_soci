cmake_minimum_required(VERSION 3.27)
project(convenienceFunctionForSoci VERSION 1.0.0
  DESCRIPTION convenienceFunctionForSoci
  LANGUAGES CXX
  ) 
cmake_policy(SET CMP0015 NEW)

find_package(Boost REQUIRED)
find_package(SOCI REQUIRED)
find_package(Catch2 REQUIRED)
find_package(magic_enum REQUIRED)


include(cmake/Cache.cmake)
set(PROJECT_WARNINGS project_warning)
add_library(${PROJECT_WARNINGS} INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_WARNINGS})
# compiler options
set(PROJECT_OPTIONS project_options)


add_library(${PROJECT_OPTIONS} INTERFACE)
target_compile_features(${PROJECT_OPTIONS} INTERFACE cxx_std_20)



include(cmake/Sanitizers.cmake)
enable_sanitizers(${PROJECT_OPTIONS})

# allow for static analysis options
#include(cmake/StaticAnalyzers.cmake) 
 
set(TARGET_NAME confu_soci)
set(SOURCES_${TARGET_NAME})
set(HEADER_${TARGET_NAME})

add_library(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

######## add sub dir
add_subdirectory(confu_soci) 

####### create executable
OPTION(BUILD_TESTS "enable tests" OFF)
IF(BUILD_TESTS)

  enable_testing()
  set(TEST_TARGET_NAME _test)
  set(TEST_SOURCES_${TARGET_NAME})
  set(TEST_HEADER_${TARGET_NAME})
  add_subdirectory(test) 
  add_executable(${TEST_TARGET_NAME}

    ${HEADER_${TARGET_NAME}}
    ${SOURCES_${TARGET_NAME}}
    ${TEST_HEADER_${TARGET_NAME}}
    ${TEST_SOURCES_${TARGET_NAME}}
  )
  target_link_libraries(${TEST_TARGET_NAME}
    ${PROJECT_OPTIONS}
          soci::soci
          SQLite::SQLite3
          Catch2::Catch2WithMain
          magic_enum::magic_enum
  )
  if (CMAKE_BINARY_DIR)
    target_compile_definitions(${TEST_TARGET_NAME} PRIVATE PATH_TO_BINARY="${CMAKE_BINARY_DIR}")
  endif()
  target_include_directories(${TEST_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

  add_test(NAME ${TEST_TARGET_NAME}
          COMMAND ${TEST_TARGET_NAME} -r junit) 
ENDIF(BUILD_TESTS)




target_sources(${TARGET_NAME} PRIVATE 
${SOURCES_${TARGET_NAME}}
${HEADER_${TARGET_NAME}}
)
####### link



target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${TARGET_NAME} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  

target_link_libraries(${TARGET_NAME} PRIVATE ${PROJECT_WARNINGS} ${PROJECT_OPTIONS}
        soci::soci
        magic_enum::magic_enum
)



#print all variables used in cmake
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
