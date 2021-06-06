##==================================================================================================
##  MUPP - MUD in C++
##  Copyright 2019-2020 Joel FALCOU
##
##  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
##  SPDX-License-Identifier: MIT
##==================================================================================================

##==================================================================================================
## Gathering compiler options for libmudpp
##==================================================================================================
add_library ( mudpp SHARED
              dummy.cpp
              # actions.cpp
              # command.cpp
              # create.cpp
              # exit.cpp
              # game.cpp
              # io.cpp
              # load.cpp
              # login.cpp
              # play.cpp
              # player.cpp
              # room.cpp
              # session.cpp
              # session_manager.cpp
              # zone.cpp
            )

add_library(mudpp::mudpp ALIAS mudpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options( mudpp PUBLIC /W3 /EHsc /std:c++latest)
else()
  target_compile_options( mudpp PUBLIC -std=c++20 -Werror -Wall -Wpedantic -Wextra)
endif()

target_compile_definitions(mudpp PRIVATE MUDPP_API_EXPORTS=1)

target_include_directories( mudpp
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${sol_SOURCE_DIR}/include
        ${tabulate_SOURCE_DIR}/include
        ${LUA_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR}
)
