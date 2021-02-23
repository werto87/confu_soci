 #set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++")
 #set(CMAKE_C_COMPILER "/usr/local/bin/clang")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
#-fcoroutines-ts -lc++experimental")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -v")
cmake_minimum_required(VERSION 3.17)
project(convenienceFunctionForSoci VERSION 1.0.0
  DESCRIPTION convenienceFunctionForSoci
  LANGUAGES CXX
  ) 
cmake_policy(SET CMP0015 NEW)




########## enable cache system
include(cmake/Cache.cmake)

#set(CMAKE_EXECUTABLE_SUFFIX ".html")
########## link against the libs to apply the configurations to the target
# compiler warnings
set(PROJECT_WARNINGS project_warning)
add_library(${PROJECT_WARNINGS} INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_WARNINGS})
# compiler options
set(PROJECT_OPTIONS project_options)


add_library(${PROJECT_OPTIONS} INTERFACE)
target_compile_features(${PROJECT_OPTIONS} INTERFACE cxx_std_20)
#target_compile_options(${PROJECT_OPTIONS} INTERFACE -ftime-trace)
# target_precompile_headers(${PROJECT_OPTIONS} INTERFACE 
# /home/walde/.conan/data/boost/1.74.0/_/_/package/23a9767f828c6565e4789a33218c8ed06b1803d6/include/boost/algorithm/string.hpp
# /home/walde/.conan/data/boost/1.74.0/_/_/package/23a9767f828c6565e4789a33218c8ed06b1803d6/include/boost/asio.hpp
# /home/walde/.conan/data/boost/1.74.0/_/_/package/23a9767f828c6565e4789a33218c8ed06b1803d6/include/boost/beast.hpp
# /home/walde/.conan/data/boost/1.74.0/_/_/package/23a9767f828c6565e4789a33218c8ed06b1803d6/include/boost/range/adaptor/indexed.hpp
# /home/walde/.conan/data/boost/1.74.0/_/_/package/23a9767f828c6565e4789a33218c8ed06b1803d6/include/boost/bind.hpp
# )


# include(cmake/Sanitizers.cmake)
# enable_sanitizers(${PROJECT_OPTIONS})

# allow for static analysis options
#include(cmake/StaticAnalyzers.cmake) 
 
######## conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


####### create executable


set(TARGET_NAME convenienceFunctionForSoci)
set(SOURCES_${TARGET_NAME})
set(HEADER_${TARGET_NAME})

add_library(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

######## add sub dir
add_subdirectory(convenienceFunctionForSoci) 

####### create executable
OPTION(BUILD_TESTS "enable tests" OFF)
IF(BUILD_TESTS)
  if (CMAKE_BINARY_DIR)
    target_compile_definitions(${TEST_TARGET_NAME} PRIVATE PATH_TO_BINARY="${CMAKE_BINARY_DIR}")
  endif()
  enable_testing()
  set(TEST_TARGET_NAME ${TARGET_NAME}_test)
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
    CONAN_PKG::catch2
    CONAN_PKG::boost
    CONAN_PKG::sqlite3
    CONAN_PKG::soci
    CONAN_PKG::magic_enum
  )
target_include_directories(${TEST_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

add_test(NAME ${TEST_TARGET_NAME}
         COMMAND ${TEST_TARGET_NAME} -r junit) 
ENDIF(BUILD_TESTS)



####### include what you use
#find_program(iwyu_path NAMES include-what-you-use iwyu)
#set_property(TARGET ${TARGET_SERVER_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
# set_property(TARGET ${TARGET_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

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
#CONAN_PKG::imgui-sfml
CONAN_PKG::boost
CONAN_PKG::sqlite3
CONAN_PKG::soci
CONAN_PKG::magic_enum
)



#print all variables used in cmake
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
