//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//=====================================================================================================
#ifndef MUDPP_API_HPP_INCLUDED
#define MUDPP_API_HPP_INCLUDED

#if defined _WIN32 || defined __CYGWIN__ || defined __MINGW32__
  #ifdef MUDPP_API_EXPORTS
    #ifdef __GNUC__
      #define MUDPP_API __attribute__ ((dllexport))
    #else
      #define MUDPP_API __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define MUDPP_API __attribute__ ((dllimport))
    #else
      #define MUDPP_API __declspec(dllimport)
    #endif
  #endif
  #define MUDPP_INTERNAL
#else
  #define MUDPP_API       __attribute__ ((visibility ("default")))
  #define MUDPP_INTERNAL  __attribute__ ((visibility ("hidden")))
#endif

#endif
