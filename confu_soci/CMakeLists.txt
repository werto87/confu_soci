add_library(confu_soci
        convenienceFunctionForSoci.cxx
        )

install(FILES
        convenienceFunctionForSoci.hxx
        concept.hxx
        DESTINATION include/confu_soci)

find_package(SOCI)
find_package(magic_enum)
find_package(Boost)


target_link_libraries(confu_soci PUBLIC
        ${PROJECT_WARNINGS}
        ${PROJECT_OPTIONS}
        boost::boost
        soci::soci
        magic_enum::magic_enum
        )

# state that anybody linking to us needs to include the current source dir
# to find MathFunctions.h, while we don't.
target_include_directories(confu_soci
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        )
install(TARGETS confu_soci DESTINATION lib)
