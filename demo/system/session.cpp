//==================================================================================================
/**
  MUPP - MUD in C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/game.hpp>

int main(int, char**)
{
  mudpp::game g( 0.1, 4000 );
  g.run();

  return 0;
}
