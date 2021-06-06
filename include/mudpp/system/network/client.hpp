//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright : MudPP Contributors & Maintainers
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#pragma once
#include <mudpp/system/network/connection.hpp>
#include <mudpp/system/network/message.hpp>
#include <mudpp/system/network/queue.hpp>
#include <string>
#include <thread>
#include <memory>

namespace mudpp::system::network
{
  template<typename T> class client
  {
    public:

    client() :socket_(context_) {}
    ~client() { disconnect(); }

    void send(packet<T> const& payload)
    {
      if(is_connected())
        connection_->send(payload);
    }

    bool connect( std::string const& host, std::uint16_t port)
    {
      try
      {
        // Where am I connecting to ?
        boost::asio::ip::tcp::resolver resolver(context_);
        auto endpoint = resolver.resolve(host, std::to_string(port));

        // Get the connection running
        connection_ = std::make_unique<connection<T>>
                      ( connection<T>::owner::client
                      , context_
                      , boost::asio::ip::tcp::socket(context_)
                      , incoming_
                      );

        connection_->connect_server(endpoint);

        context_thread_ = std::thread( [this]() { context_.run(); });
      }
      catch(std::exception& e)
      {
        std::cerr << "[mudpp][network][client::connect] Exception: " << e.what() << std::endl;
        return false;
      }

      return true;
    }

    void disconnect()
    {
      if( is_connected()) connection_->disconnect();
      context_.stop();

      if(context_thread_.joinable()) context_thread_.join();
      connection_.release();
    }

    bool is_connected() const
    {
      if( connection_)  return connection_->is_connected();
      else              return false;
    }

    auto& incoming() { return incoming_; }

    protected:
    boost::asio::io_context         context_;
    std::thread                     context_thread_;
    boost::asio::ip::tcp::socket    socket_;
    std::unique_ptr<connection<T>>  connection_;

    private:
    safe_queue<message<T>>         incoming_;
  };
}
