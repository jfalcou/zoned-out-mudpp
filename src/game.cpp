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
  {
    lua_setup();
  }

  void game::shutdown()
  {
    // Notify of shutdown
    sessions_.broadcast(urgent("**Server is shutting down NOW!**"));

    // Stop I/O processing
    ios_.stop();
  }

  void game::spawn_player(session& s)
  {
    players_.push_back( player::make(s) );
    log(std::cout,"GAME") << "New player connected." << std::endl;
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

  bool game::run()
  {
    log(std::cout,"GAME") << "Start..." << std::endl;

    // Setup game events
    register_event( 5000, [this, msg = info("**TICK**")]() { sessions_.broadcast(msg); } );

    // Start the game
    auto tic = std::chrono::steady_clock::now();

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

      } while( !ios_.stopped() );
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
}
