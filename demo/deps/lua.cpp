//==================================================================================================
/**
  MUPP - MUD in C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================

#include <sol/sol.hpp>

int main(int argc, char *argv[])
{
  sol::state lua;
  lua.open_libraries(sol::lib::base);

  lua.script("print('bark bark bark!')");

  return 0;
}
