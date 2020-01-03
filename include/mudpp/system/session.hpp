//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

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
  struct game;
  struct player;

  struct session
  {
    public:
    session(game& g);

    boost::asio::ip::tcp::socket&        socket()        { return socket_; }
    boost::asio::ip::tcp::socket const&  socket() const  { return socket_; }

    void start();
    void disconnect();
    void tick();

    void send( std::string const& msg ) { outgoing_message_ += msg;   }
    bool is_valid() const               { return connection_status_;  }

    game& context() { return game_context_; }

    private:
    void read ( const boost::system::error_code& error, std::size_t size);
    void write( const boost::system::error_code& error, std::size_t size);

    game&                         game_context_;
    player*                       active_player_;
    boost::asio::ip::tcp::socket  socket_;
    bool                          connection_status_;
    std::array<char,512>          buffer_;
    std::string                   incoming_message_;
    std::string                   outgoing_message_;
  };

  // Local using for session resource pointer
  using session_t = std::unique_ptr<session>;
}

#endif
