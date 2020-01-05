//==================================================================================================
/**
  MUPP - MUD in C++
  Copyright 20 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/game.hpp>

int main(int, char**)
{
  mudpp::game g( "config/config.lua" );
  g.run();

  return 0;
}
