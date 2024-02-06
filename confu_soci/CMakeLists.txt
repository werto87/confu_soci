##
## Copyright (c) 2024 Waldemar Schneider (w-schneider1987 at web dot de)
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


add_library(confu_soci convenienceFunctionForSoci.cxx)


add_library(myproject::confu_soci ALIAS confu_soci)


find_package(SOCI)
find_package(magic_enum)
find_package(Boost)
find_package(SQLite3)


target_link_libraries(confu_soci PUBLIC
        SQLite::SQLite3
        soci::soci
        magic_enum::magic_enum
        boost::boost
        )

target_include_directories(confu_soci
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        )

target_compile_features(confu_soci PUBLIC cxx_std_20)

set_target_properties(
        confu_soci
        PROPERTIES VERSION ${PROJECT_VERSION}
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES)

install(FILES
        convenienceFunctionForSoci.hxx
        concept.hxx
        DESTINATION include/confu_soci
        )
install(TARGETS confu_soci DESTINATION lib)
