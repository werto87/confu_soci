set(TARGET_NAME_LIB confu_soci)
add_library(${TARGET_NAME_LIB}
        convenienceFunctionForSoci.cxx
        )

target_link_libraries(${TARGET_NAME_LIB} PUBLIC
         ${PROJECT_WARNINGS}
         ${PROJECT_OPTIONS}
         boost::boost
         soci::soci
         magic_enum::magic_enum
         )

# state that anybody linking to us needs to include the current source dir
# to find MathFunctions.h, while we don't.
target_include_directories(${TARGET_NAME_LIB}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        )