//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/game.hpp>
#include <mudpp/engine/player.hpp>
#include <mudpp/system/timestamp.hpp>
#include <mudpp/system/session.hpp>
#include <mudpp/system/io.hpp>
#include <tabulate/termcolor.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>

namespace mudpp
{
  game::game( double freq, int port )
      : ios_()
      , events_(), players_()
      , sessions_(*this,4000)
      , frequency_(freq), elapsed_(0.0)
      , shutdown_(false)
  {
    lua_setup();
  }

  void game::shutdown()
  {
    shutdown_ = true;
  }

  std::ostream& game::log(std::ostream& os, std::string const& context)
  {
    os  << termcolor::bold << timestamp
        << "[";

    if(&os == &std::cerr) os << termcolor::red;
    else                  os << termcolor::blue;

    os << context << termcolor::reset << termcolor::bold << "] - " << termcolor::reset;
    return os;
  }

  player* game::find_player(std::string const& name)
  {
    auto p = std::find_if ( players_.begin(),players_.end()
                          , [&](auto const& e) { return e->name() == name; }
                          );

    return (p != players_.end()) ? p->get() : nullptr;
  }

  bool game::run()
  {
    log(std::cout,"GAME") << "Start..." << std::endl;

    // Start the game
    auto tic = std::chrono::steady_clock::now();

    // Setup game events
    register_event( 5000, [this]() { for(auto& p : players_) p->tick(); } );

    try
    {
      do
      {
        // Process message if any
        ios_.poll();

        // Check if it's time to update the game
        auto toc = std::chrono::steady_clock::now();
        elapsed_ = (toc - tic).count();

        if(elapsed_ > frequency_)
        {
          tic = toc;
          // Update game state

          // Propagate updates through all open sessions
          sessions_.tick();
        }

      } while( !shutdown_ );

      // Notify of shutdown
      sessions_.broadcast(urgent("**Server is shutting down NOW!**"));
    }
    catch(std::exception& e)
    {
      log(std::cerr,"GAME") << "Unexpected error: " << e.what() << std::endl;
      log(std::cout,"GAME") << "Shutting down..." << std::endl;

      return false;
    }

    log(std::cout,"GAME") << "Shutting down..." << std::endl;

    return true;
  }

  void game::lua_setup()
  {
    lua_state_.open_libraries ( sol::lib::base, sol::lib::io, sol::lib::string
                              , sol::lib::math, sol::lib::os
                              );

    // Provide access to game::log from LUA
    lua_state_.set_function ( "mudpp_log"
                            , [&](std::string const& s) { log(std::cout,"LUA") << s << std::endl; }
                            );

    // Sanity check
    lua_state_.script ( "mudpp_log('LUA engine started.')" );
  }

  player* game::attach_player(session& s)
  {
    log(std::cout,"GAME") << "New player connected." << std::endl;
    players_.push_back(player::make(s));
    return players_.back().get();
  }
}
