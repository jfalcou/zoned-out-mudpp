//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_GAME_HPP_INCLUDED
#define MUDPP_ENGINE_GAME_HPP_INCLUDED


namespace mudpp
{
  struct game
  {
    game(int port = 4000) : sessions_(4000) {}

    private:
    mudpp::session_manager sessions_;
  };
}

#endif
