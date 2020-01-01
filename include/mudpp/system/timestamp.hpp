//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_SYSTEM_TIMESTAMP_HPP_INCLUDED
#define MUDPP_SYSTEM_TIMESTAMP_HPP_INCLUDED

#include <iostream>
#include <iomanip>
#include <chrono>

namespace mudpp
{
  struct timestamp_ {};
  constexpr timestamp_ timestamp = {};

  inline std::ostream& operator<<(std::ostream& os, timestamp_ const&)
  {
    std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    os << "[" << std::put_time(std::localtime(&now_c), "%F %T") << "]";
    return os;
  }
}

#endif
