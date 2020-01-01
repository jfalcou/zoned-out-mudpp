//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_SYSTEM_SESSION_MANAGER_HPP_INCLUDED
#define MUDPP_SYSTEM_SESSION_MANAGER_HPP_INCLUDED

#include <mudpp/system/periodic_event.hpp>
#include <mudpp/system/session.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <vector>

namespace mudpp
{
  struct game;

  struct session_manager
  {
    public:
    session_manager(game& g, int port);
    void tick();
    void broadcast( std::string const& msg);

    private:
    using endpoint_t = boost::asio::ip::tcp::endpoint;

    void accept();
    void cleanup();
    void stats();

    game&                           game_context_;
    boost::asio::ip::tcp::acceptor  acceptor_;
    std::vector<session_t>          sessions_;
  };
}

#endif
