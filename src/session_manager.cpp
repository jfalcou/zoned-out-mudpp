//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/system/session_manager.hpp>
#include <mudpp/engine/game.hpp>
#include <tabulate/termcolor.hpp>
#include <iostream>
#include <cstdint>
#include <memory>

namespace mudpp
{
  session_manager::session_manager(game& g, int port)
                  : game_context_(g)
                  , acceptor_(game_context_.io(), endpoint_t(boost::asio::ip::tcp::v4(), port))
  {
    // Cleanup unused sessions every 0.5s
    game_context_.register_event( 500, [this]() { cleanup(); } );

    // Prompt a stat of the server every 5s
    game_context_.register_event( 5000, [this]() { stats(); } );

    // Starts accepting sessions
    accept();
  }

  void session_manager::accept()
  {
    // Make a new session
    sessions_.push_back(std::make_unique<session>(game_context_));

    // Whenever something happens, start the session and wait for acceptance again
    acceptor_.async_accept( sessions_.back()->socket()
                          , [this](auto const& error) { sessions_.back()->start(); accept(); }
                          );
  }

  // Go over every sessions and remove/erase all that are invalid
  void session_manager::cleanup()
  {
    auto old_count = sessions_.size();

    sessions_.erase( std::remove_if ( sessions_.begin(), sessions_.end()
                                    , [](auto const& s) { return !s->is_valid(); }
                                    )
                    , sessions_.end()
                    );

    auto diff = old_count - sessions_.size();

    if(diff)
      game_context_.log(std::cout,"NETWORK") << diff << " connections cleaned up." << std::endl;
  }

  // Live info display
  void session_manager::stats()
  {
    game_context_.log(std::cout,"NETWORK") << sessions_.size()-1 << " alive sessions." << std::endl;
  }

  // Publish a *tick* to all player
  void session_manager::tick()
  {
    for(auto& s : sessions_)
      if(s->is_valid())
        s->tick();
  }

  void session_manager::broadcast(std::string const& msg)
  {
    for(auto& s : sessions_)
      if(s->is_valid())
        s->send(msg);

    tick();
  }
}
