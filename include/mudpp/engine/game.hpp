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

#include <mudpp/system/session_manager.hpp>
#include <boost/asio.hpp>

namespace mudpp
{
  struct game
  {
    game(int port = 4000) : sessions_(ios_,4000) {}

    void run()
    {
      ios_.run();
    }

    private:
    boost::asio::io_service ios_;
    mudpp::session_manager  sessions_;
  };
}

#endif
