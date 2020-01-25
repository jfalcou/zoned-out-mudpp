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
#include <boost/asio.hpp>
#include <iostream>

namespace mudpp
{
  game::game( std::string const& config_file )
      : ios_(), events_(), players_(), shutdown_(false)
  {
    // Setup LUA
    setup_scripting(config_file);

    // Build session
    int port  = lua_state_["port"];
    sessions_ = std::make_unique<session_manager>(*this,port);

    // Read messages
    lua_state_.script_file( paths_.value()["strings"].c_str());
    messages_ = lua_state_["messages"];
  }

  void game::shutdown()
  {
    shutdown_ = true;
  }

  bool game::exists( player const& p)
  {
    return std::find_if ( players_.begin(), players_.end()
                        , [&](auto const& e) { return e->name() == p.name() && e->is_logged(); }
                        ) != players_.end();
  }

  std::ostream& game::log(std::ostream& os, std::string const& context)
  {
    os << timestamp << "[" << context << "] - ";
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

    // Setup game events
    register_event( lua_state_["tick_period"], [this]() { for(auto& p : players_) p->tick(); } );

    // Cleanup resources
    register_event( lua_state_["cleanup_period"], [this]() { cleanup(); } );

    // Display network stats
    register_event( lua_state_["stats_period"], [this]() { sessions_->stats(); } );

    try
    {
      // Start the game
      auto tic = std::chrono::high_resolution_clock::now();

      do
      {
        // Process message if any
        ios_.poll();

        // Check if it's time to update the game
        auto toc = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = toc - tic;

        if(elapsed.count() > period_)
        {
          tic = toc;
          // Update game state

          // Propagate updates through all open sessions
          sessions_->tick();
        }

      } while( !shutdown_ );

      // Notify of shutdown
      sessions_->broadcast(urgent("**Server is shutting down NOW!**"));
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

  // Go over every sessions and remove/erase all that are invalid
  void game::cleanup()
  {
    auto old_count = players_.size();

    players_.erase( std::remove_if ( players_.begin(), players_.end()
                                    , [](auto const& s) { return !s->is_connected(); }
                                    )
                    , players_.end()
                    );

    auto diff = old_count - players_.size();

    if(diff)
      log(std::cout,"GAME") << diff << " players cleaned up." << std::endl;

    sessions_->cleanup();
  }

  void game::setup_scripting( std::string const& config_file )
  {
    lua_state_.open_libraries ( sol::lib::base, sol::lib::io, sol::lib::string
                              , sol::lib::math, sol::lib::os, sol::lib::package
                              , sol::lib::table
                              );

    // Provide access to game::log from LUA
    lua_state_.set_function ( "mudpp_log"
                            , [&](std::string const& s) { log(std::cout,"LUA") << s << std::endl; }
                            );

    // Provide access to game::exists from LUA
    lua_state_.set_function ( "mudpp_player_exists"
                            , [&](player const& p) { return exists( p ); }
                            );

    // Provide access to a "build a colored text" for lUA
    lua_state_.set_function ( "mudpp_message"
                            , [&](std::string const& s) { return colorize(s); }
                            );

    // Sanity check
    lua_state_.script ( "mudpp_log('LUA engine started.')" );

    // Read config
    lua_state_.script_file(config_file.c_str());
    paths_      = lua_state_["path"];
    period_     = lua_state_["base_period"];

    // Perform scripting setup for other types
    player::setup_scripting(player_type, lua_state_);
    lua_state_.script_file( paths()["player"].c_str() );
  }

  player* game::attach_player(session& s)
  {
    log(std::cout,"GAME") << "New player connected." << std::endl;
    players_.push_back(player::make(s));

    auto& p = players_.back();
    auto b = box_message( {"@", tabulate::Color::yellow}
                        , {"¤", tabulate::Color::yellow}
                        , { {tabulate::FontStyle::bold}
                          , tabulate::Color::red
                          , tabulate::FontAlign::center
                          }
                        , 100
                        , strings()["welcome"]
                        );

    p->send(b,false);
    p->login_prompt();

    return p.get();
  }
}
