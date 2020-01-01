//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/system/session.hpp>
#include <mudpp/engine/game.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace mudpp
{
  session::session(game& g)
          : game_context_(g), socket_(g.io()), connection_status_(true)
  {
  }

  void session::tick()
  {
    // At each tick, we try to flush out any outgoing message that still lingers
    if(!outgoing_message_.empty())
    {
      boost::asio::async_write( socket_
                              , boost::asio::buffer(outgoing_message_)
                              , [this](auto const& e, auto sz) { write(e,sz); }
                              );
    }
  }

  void session::start()
  {
    game_context_.log(std::cout,"NETWORK")  << "Connection from: "
                                            << socket_.remote_endpoint() << std::endl;

    // Bind current session to a new player in game_context

    // Ask for player ID and process login

    // Start input processing
    socket_.async_read_some ( boost::asio::buffer(buffer_.data(), buffer_.size())
                            , [this](auto const& e, auto sz) { read(e,sz); }
                            );
  }

  void session::read(const boost::system::error_code& error, std::size_t size)
  {
    if( !error && is_valid() )
    {
      // Enqueue data to the incoming message string
      incoming_message_ += std::string(buffer_.begin(),buffer_.begin() + size);

      // try to extract lines from the input buffer
      while(true)
      {
        // Split message buffer on carriage return
        auto i = incoming_message_.find('\n');

        // Exit the loop if no more lines are available
        if(i == std::string::npos)
          break;

        // Extract line from the byte buffer
        auto line = incoming_message_.substr(0, i);
        boost::trim_if(line, boost::is_any_of("\t\r\n"));

        // Reshape remaining incoming message buffer
        incoming_message_ = incoming_message_.substr(i + 1);

        game_context_.log(std::cout,"INPUT") << "Processing: '"<< line << "'" << std::endl;

        // now, do something with it -> ProcessPlayerInput (this, Trim (line));
        if(line == "/quit")
        {
          game_context_.log(std::cout,"NETWORK")  << "Connection from: "
                                                  << socket_.remote_endpoint()
                                                  << " disconnected." << std::endl;

          connection_status_ = false;
          outgoing_message_ += "Bye !!\n";
        }
        else if(line == "/shutdown")
        {
          game_context_.shutdown();
        }
        else if(line == "/hello")
        {
          outgoing_message_ += "Hello player !\n";
        }
        else
        {
          outgoing_message_ += "No comprendo senor!\n";
        }
      }

      // Read next bits of data
      socket_.async_read_some ( boost::asio::buffer(buffer_.data(), buffer_.size())
                              , [this](auto const& e, auto sz) { read(e,sz); }
                              );

      // Did we generated some output to process ?
      if(!outgoing_message_.empty())
      {
        boost::asio::async_write( socket_
                                , boost::asio::buffer(outgoing_message_)
                                , [this](auto const& e, auto sz) { write(e,sz); }
                                );
      }
    }
    else
    {
      // Error causes disconnection
      if ((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error))
      {
        game_context_.log(std::cerr,"NETWORK")  << "Connection from: "
                                                << socket_.remote_endpoint()
                                                << " terminated." << std::endl;
        connection_status_ = false;
      }
    }
  }

  void session::write(const boost::system::error_code& error, std::size_t sz)
  {
    // Error causes disconnection
    if ((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error))
    {
      game_context_.log(std::cerr,"NETWORK")  << "Connection from: "
                                              << socket_.remote_endpoint()
                                              << " terminated." << std::endl;
      connection_status_ = false;
      return;
    }

    // Cleanup sent data
    outgoing_message_.erase(0,sz);

    // Return to listening state
    socket_.async_read_some ( boost::asio::buffer(buffer_.data(), buffer_.size())
                            , [this](auto const& e, auto sz) { read(e,sz); }
                            );
    }
}
