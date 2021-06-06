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
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <thread>


namespace mudpp::system::network
{
  template<typename T>
  class server
  {
    public:
    server(std::uint16_t port)
      : acceptor_ ( context_
                  , boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4()
                                                  , port
                                                  )
                  )
    {
    }

    virtual ~server() { stop(); }

    bool start()
    {
      try
      {
        async_wait_connection();
        context_thread_ = std::thread( [this]() { context_.run(); });
      }
      catch( std::exception& e)
      {
        std::cerr << "[mudpp][network][server::start] Exception: " << e.what() << std::endl;
        return false;
      }

      std::cout << "[mudpp][network][server::start] Started with success" << std::endl;
      return true;
    }

    void stop()
    {
      context_.stop();

      if(context_thread_.joinable())
        context_thread_.join();

      std::cout << "[mudpp][network][server::stop] Stopped with success" << std::endl;
    }

    void async_wait_connection()
    {
      acceptor_.async_accept(
        [this](auto ec, auto socket)
        {
          if(!ec)
          {
            std::cout << "[mudpp][network][server::connection] Connection from: "
                      << socket.remote_endpoint()
                      << std::endl;

            auto new_connection = std::make_shared<connection<T>> ( connection<T>::owner::server
                                                                  , context_
                                                                  , std::move(socket)
                                                                  , incoming_
                                                                  );

            if( on_connect(new_connection))
            {
              connections_.push_back(std::move(new_connection));
              connections_.back()->connect_client(id_counter_++);
              std::cout << "[mudpp][network][server::connection] Connection "
                        << connections_.back()->id() << " approved"
                        << std::endl;
            }
            else
            {
              std::cout << "[mudpp][network][server::connection] Connection denied"
                        << std::endl;
            }
          }
          else
          {
            std::cerr << "[mudpp][network][server::connection] Exception: "
                      << ec.message()
                      << std::endl;
          }

          async_wait_connection();
        }
      );
    }

    void send_to( std::shared_ptr<connection<T>>& client
                , packet<T> const& payload
                )
    {
      if(client && client->is_connected())
      {
        client->send(payload);
      }
      else
      {
        on_disconnect(client);
        client.reset();

        // Remove from the set of connections
        connections_.erase( std::remove(connections_.begin(),connections_.end(), client)
                          , connections_.end()
                          );
      }
    }

    void broadcast( packet<T> const& payload
                  ,  std::shared_ptr<connection<T>> const& ignore = nullptr
                  )
    {
      bool has_invalids = false;

      for(auto& client : connections_)
      {
        if(client && client->is_connected())
        {
          if(client != ignore)
            client->send(payload);
        }
        else
        {
          on_disconnect(client);
          client.reset();
          has_invalids = true;
        }
      }

      // Remove all invalids from the set of connections at once
      if(has_invalids)
        connections_.erase( std::remove(connections_.begin(),connections_.end(), nullptr)
                          , connections_.end()
                          );
    }

    // Let's control message process from outside
    void update(std::size_t nb_messages = -1)
    {
      incoming_.wait();
      std::size_t message_count = 0;
      while(message_count < nb_messages && !incoming_.empty())
      {
        auto msg = incoming_.pop_front();
        on_message(msg.remote, msg.payload);
        message_count++;
      }
    }

    protected:
    virtual bool on_connect(std::shared_ptr<connection<T>> /*client*/)
    {
      return false;
    }

    virtual void on_disconnect(std::shared_ptr<connection<T>> /*client*/)
    {
    }

    virtual void on_message ( std::shared_ptr<connection<T>> /*client*/
                            , packet<T> const& /*payload*/
                            )
    {

    }

    safe_queue<message<T>>                      incoming_;
    std::deque<std::shared_ptr<connection<T>>>  connections_;
    boost::asio::io_context                     context_;
    std::thread                                 context_thread_;

    boost::asio::ip::tcp::acceptor  acceptor_;
    std::uint32_t id_counter_ = 10'000;

    private:
  };
}
