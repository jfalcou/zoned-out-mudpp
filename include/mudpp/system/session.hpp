//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_SYSTEM_SESSION_HPP_INCLUDED
#define MUDPP_SYSTEM_SESSION_HPP_INCLUDED

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <string>
#include <array>

namespace mudpp
{
  class session_manager;

  class session
  {
    public:
    session(boost::asio::io_service& io_service, session_manager* parent);

    boost::asio::ip::tcp::socket&        socket()        { return socket_; }
    boost::asio::ip::tcp::socket const&  socket() const  { return socket_; }

    void start();
    bool is_valid() const { return connection_status_; }

    private:
    void handle_read  ( const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_write ( const boost::system::error_code& error);

    session_manager*              manager_;
    boost::asio::io_service&      ios_;
    boost::asio::ip::tcp::socket  socket_;
    bool                          connection_status_;
    std::array<char,1026>         buffer_;
  };

  // Local using for session resource pointer
  using session_t = std::unique_ptr<session>;
}

#endif
