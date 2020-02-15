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

  bool game::exists( std::string const& name )
  {
    return std::find_if ( players_.begin(), players_.end()
                        , [&](auto const& e) { return e->name() == name && e->is_logged(); }
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

  void game::load_zones()
  {
    std::string zones  = paths()["asset"];
                zones += "zones.lua";

    lua_state_.script_file( zones.c_str() );
    sol::table zs = lua_state_["zones"];

    for (auto it : zs)
    {
      int        id = it.first.as<int>();
      sol::table z  = it.second.as<sol::table>();
      zones_.insert({ id, zone(*this,id,z)});
    }

    log(std::cout,"GAME") << "Loaded: " << zones_.size() << " zones." << std::endl;
  }

  bool game::run()
  {
    log(std::cout,"GAME") << "Start..." << std::endl;

    // Setup game events
    register_event( period_, [this]() { for(auto& p : players_) p->tick(); } );

    // Cleanup resources
    register_event( lua_state_["cleanup_period"], [this]() { cleanup(); } );

    // Display network stats
    register_event( lua_state_["stats_period"], [this]() { sessions_->stats(); } );

    // Load zones
    load_zones();

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
    }
    catch(std::exception& e)
    {
      log(std::cerr,"GAME") << "Unexpected error: " << e.what() << std::endl;
      log(std::cout,"GAME") << "Shutting down..." << std::endl;

      return false;
    }

    // Propagate updates through all open sessions to flush last messages
    sessions_->tick();
    log(std::cout,"GAME") << "Shutting down..." << std::endl;

    return true;
  }

  void game::broadcast(std::string const& message)
  {
    std::for_each ( players_.begin(), players_.end()
                  , [&](auto const& p) { if(p->is_connected()) p->send(message); }
                  );
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
    // Prepare LUA
    lua_state_.open_libraries ( sol::lib::base, sol::lib::io, sol::lib::string
                              , sol::lib::math, sol::lib::os, sol::lib::package
                              , sol::lib::table
                              );

    system_module_ = lua_state_["game"].force();

    // Provide access to game::log from LUA
    system_module_.set_function ( "log"
                                , [&](std::string const& s) { log(std::cout,"LUA") << s << std::endl; }
                                );

    // Provide access to game::exists from LUA
    system_module_.set_function ( "player_exists", [&](std::string const& p) { return exists( p ); } );

    // Sanity check
    lua_state_.script ( "game.log('LUA engine started.')" );

    // Read config
    lua_state_.script_file(config_file.c_str());
    paths_      = lua_state_["path"];
    period_     = lua_state_["base_period"];

    // Perform scripting setup for other types
    player::setup_scripting (player_type_ , lua_state_);
    room::setup_scripting   (room_type_   , lua_state_);
    zone::setup_scripting   (zone_type_   , lua_state_);

    lua_state_.script_file( paths()["player"].c_str() );
  }

  player* game::attach_player(session& s)
  {
    log(std::cout,"GAME") << "New player connected." << std::endl;
    players_.push_back(player::make(s));

    auto& p = players_.back();
    p->login();

    return p.get();
  }

  room* game::find_room(int id)
  {
    int zone_id = id / 1000;
    int room_id = id % 1000;

    auto z = zones_.find(zone_id);
    if(z != zones_.end())
    {
      return z->second.find_room(room_id);
    }
    else
    {
      return nullptr;
    }
  }

}
