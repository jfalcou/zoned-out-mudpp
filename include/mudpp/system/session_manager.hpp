//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_SYSTEM_SESSION_MANAGER_HPP_INCLUDED
#define MUDPP_SYSTEM_SESSION_MANAGER_HPP_INCLUDED

#include <mudpp/system/periodic_event.hpp>
#include <mudpp/system/session.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <vector>

namespace mudpp
{
  class session_manager
{
  public:
  session_manager(boost::asio::io_service& io_service, uint16_t port);

  private:
  void start_accept();

  boost::asio::io_service&              io_service;
  boost::asio::ip::tcp::acceptor        acceptor;
  periodic_event                        event;
  std::vector<std::unique_ptr<session>> sessions;
};
}

#endif
