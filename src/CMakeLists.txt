##==================================================================================================
##  MUPP - MUD in C++
##  Copyright 2019-2020 Joel FALCOU
##
##  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
##  SPDX-License-Identifier: MIT
##==================================================================================================

add_library ( mudpp SHARED
              actions.cpp
              command.cpp
              create.cpp
              game.cpp
              io.cpp
              load.cpp
              login.cpp
              play.cpp
              player.cpp
              room.cpp
              session.cpp
              session_manager.cpp
              zone.cpp
            )

if( MSVC )
  set( options /std:c++latest /W3 /EHsc)
else()
  set( options -std=c++17 -Wall -Wno-missing-braces -fvisibility=hidden)
endif()

add_library(mudpp::mudpp ALIAS mudpp)

target_compile_options( mudpp PRIVATE ${options} )
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
