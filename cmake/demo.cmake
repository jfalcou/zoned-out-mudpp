##==================================================================================================
##  MUPP - MUD in C++
##  Copyright 2019-2020 Joel FALCOU
##
##  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
##  SPDX-License-Identifier: MIT
##==================================================================================================

include(add_parent_target)

##==================================================================================================
## Centralize all required setup for unit tests
##==================================================================================================
function(add_demo root)
  if( MSVC )
    set( options /std:c++latest -W3 -EHsc)
  else()
    set( options -std=c++17 -Wall -Wno-missing-braces -DSOL_ALL_SAFETIES_ON=1)
  endif()

  foreach(file ${ARGN})
    string(REPLACE ".cpp" ".demo" base ${file})
    string(REPLACE "/"    "." base ${base})
    string(REPLACE "\\"   "." base ${base})
    set(test "${root}.${base}")

    add_executable( ${test}  ${file})
    target_compile_options  ( ${test} PUBLIC ${options} )

    set_property( TARGET ${test}
                  PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/game"
                )

    set_target_properties ( ${test} PROPERTIES
                            EXCLUDE_FROM_DEFAULT_BUILD TRUE
                            EXCLUDE_FROM_ALL TRUE
                            ${MAKE_UNIT_TARGET_PROPERTIES}
                          )

    target_include_directories( ${test}
                                PRIVATE
                                ${sol_SOURCE_DIR}/include
                                ${tabulate_SOURCE_DIR}/include
                                ${PROJECT_SOURCE_DIR}/demo
                                ${PROJECT_SOURCE_DIR}/include
                                ${LUA_INCLUDE_DIR}
                              )

    target_link_libraries ( ${test}
                            mudpp
                            ${MUPP_LIBRARIES}
                            Threads::Threads
                          )

    add_dependencies(game ${test})

    add_parent_target(${test})
  endforeach()
endfunction()

## Setup our tests
add_custom_target(game)
add_subdirectory(${PROJECT_SOURCE_DIR}/game/)
