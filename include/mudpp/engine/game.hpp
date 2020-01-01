//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_GAME_HPP_INCLUDED
#define MUDPP_ENGINE_GAME_HPP_INCLUDED

#include <mudpp/system/session_manager.hpp>
#include <mudpp/engine/player.hpp>
#include <sol/sol.hpp>
#include <boost/asio.hpp>

namespace mudpp
{
  struct session;

  struct game
  {
    game( double freq = 0.1, int port = 4000 );

    void shutdown();
    bool run();

    void spawn_player(session&);

    boost::asio::io_service& io() { return ios_; }

    template<typename Function> void register_event(int period, Function f)
    {
      events_.push_back( periodic_event::make(ios_, period, f) );
    }

    std::ostream& log(std::ostream& os, std::string const& context);

    private:

    void lua_setup();

    boost::asio::io_service       ios_;
    std::vector<periodic_event_t> events_;
    std::vector<player_t>         players_;
    mudpp::session_manager        sessions_;
    sol::state                    lua_state_;
    double                        frequency_;
    double                        elapsed_;
  };

}

#endif
