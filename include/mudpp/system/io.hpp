//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <tabulate/termcolor.hpp>
#include <sstream>

namespace mudpp
{
  template<typename T> inline std::string urgent(T const& data)
  {
    std::ostringstream stream;
    stream  << termcolor::colorize << termcolor::red << termcolor::bold
            << data << termcolor::reset << '\n';

    return stream.str();
  }

  template<typename T> inline std::string info(T const& data)
  {
    std::ostringstream stream;
    stream  << termcolor::colorize << termcolor::yellow << termcolor::bold
            << data << termcolor::reset << '\n';

    return stream.str();
  }
}
