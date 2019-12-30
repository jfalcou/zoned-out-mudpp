##==================================================================================================
##  MUPP - MUD in C++
##  Copyright 2019 Joel FALCOU
##
##  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
##  SPDX-License-Identifier: MIT
##==================================================================================================
cmake_minimum_required(VERSION 3.2)
## =================================================================================================
## Setup project
## =================================================================================================
project(mupp LANGUAGES CXX)
set(PROJECT_VERSION 0.0.0.1)
set(PROJECT_DOC_DIR   ${PROJECT_SOURCE_DIR}/doc)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake )

## =================================================================================================
## No in-source build
## =================================================================================================
if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  message( FATAL_ERROR "[mupp] Building in-source, not recommended! Build in a separate directory." )
endif()

## =================================================================================================
## Options
## =================================================================================================
option( MUPP_BUILD_TEST  "Build tests for mupp" ON  )
message( STATUS "[mupp] Building ${CMAKE_BUILD_TYPE} mode with: ${CMAKE_CXX_FLAGS}")

## =================================================================================================
## Dependencies
## =================================================================================================
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads              REQUIRED        )
find_package(Lua    "5.3"   EXACT REQUIRED        )

include(download)

download_project( PROJ                sol
                  GIT_REPOSITORY      https://github.com/ThePhD/sol2.git
                  GIT_TAG             develop
                  "UPDATE_DISCONNECTED 1"
                  #QUIET
                )

set(MUPP_LIBRARIES ${LUA_LIBRARIES})

## =================================================================================================
## libmupp
## =================================================================================================
add_subdirectory(src)

## =================================================================================================
## Install target
## =================================================================================================
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include DESTINATION .)

## =================================================================================================
## Demo targets
## =================================================================================================
include(demo)
