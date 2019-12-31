//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/system/session_manager.hpp>
#include <iostream>
#include <cstdint>
#include <memory>

namespace mudpp
{
  session_manager::session_manager(boost::asio::io_service& ios, int port)
                  : ios_(ios), acceptor_(ios, endpoint_t(boost::asio::ip::tcp::v4(), port))
  {
    // Cleanup unused sessions every 5s
    events_.push_back( periodic_event::make(ios_, 5000, [this]() { cleanup(); } ) );

    // Prompt a stat of the server every 1s
    events_.push_back( periodic_event::make(ios_, 1000, [this]() { stats();   } ) );

    // Starts accepting sessions
    accept();
  }

  void session_manager::accept()
  {
    // Make a new session
    sessions_.push_back(std::make_unique<session>(ios_,this));

    // Whenever something happens, start the session and wait for acceptance again
    acceptor_.async_accept( sessions_.back()->socket()
                          , [this](auto const& error) { sessions_.back()->start(); accept(); }
                          );
  }

  // Go over every sessions and remove/erase all that are invalid
  void session_manager::cleanup()
  {
    auto old_count = sessions_.size() - 1;

    sessions_.erase( std::remove_if ( sessions_.begin(), sessions_.end()
                                    , [](auto const& s) { return !s->is_valid(); }
                                    )
                    , sessions_.end()
                    );

    auto new_count = sessions_.size() - 1;

    std::cout << "[CLEANUP] - " << (old_count - new_count) << " sessions cleaned up.\n";
  }

  // Live info display
  void session_manager::stats()
  {
    std::cout << "[INFO] - " << sessions_.size()-1 << " alive sessions." << std::endl;
  }
}
