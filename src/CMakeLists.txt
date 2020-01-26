##==================================================================================================
##  MUPP - MUD in C++
##  Copyright 2019-2020 Joel FALCOU
##
##  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
##  SPDX-License-Identifier: MIT
##==================================================================================================

add_library ( mupp
              game.cpp
              io.cpp
              player.cpp
              room.cpp
              session.cpp
              session_manager.cpp
              zone.cpp
            )

if( MSVC )
  set( options /std:c++latest /W3 /EHsc)
else()
  set( options -std=c++17 -Wall -Wno-missing-braces )
endif()

target_include_directories(mupp
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

target_compile_options( mupp PRIVATE ${options} )
