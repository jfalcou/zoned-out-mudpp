//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright : MudPP Contributors & Maintainers
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#pragma once
#include <mudpp/system/network/message.hpp>
#include <mudpp/system/network/packet.hpp>
#include <mudpp/system/network/queue.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <memory>
#include <iostream>

namespace mudpp::system::network
{

  template<typename T>
  class connection : public std::enable_shared_from_this<connection<T>>
  {
    public:

    enum class owner { server, client };

    connection( owner                           o
              , boost::asio::io_context&        ctx
              , boost::asio::ip::tcp::socket&&  socket
              , safe_queue<message<T>>&         in
              )
              : context_(ctx)
              , socket_( std::move(socket) )
              , incoming_(in)
     {
        ownership_ = o;
     }

    ~connection() {}

    void connect_server(boost::asio::ip::tcp::resolver::results_type& ep)
    {
      if( ownership_ == owner::client )
      {
        boost::asio::async_connect( socket_, ep
                        , [this](auto ec, auto)
                          {
                            if(!ec) async_read_header();
                          }
                        );
      }
    }

    void connect_client(std::uint32_t id = 0)
    {
      if( ownership_ == owner::server )
      {
        if(socket_.is_open())
        {
          id_ = id;
          async_read_header();
        }
      }
    }

    void disconnect()
    {
      if(is_connected())
      {
        boost::asio::post(context_, [this](){ socket_.close();});
      }
    }

    bool is_connected() const
    {
      return socket_.is_open();
    }

    void send(packet<T> const& payload)
    {
      boost::asio::post ( context_
                        , [this, payload]()
                        {
                          bool has_msg = !outgoing_.empty();
                          outgoing_.push_back(payload);

                          if(!has_msg)
                          {
                            async_write_header();
                          }
                        }
                        );
    }

    auto id() const { return id_; }

    private:

    void async_read_header()
    {
      boost::asio::
      async_read( socket_
                , boost::asio::buffer ( &buffer_.header
                                      , sizeof( packet_header<T> )
                                      )
                , [this](auto ec, std::size_t)
                {
                  if(!ec)
                  {
                    if(buffer_.header.size > 0)
                    {
                      buffer_.body.resize(buffer_.header.size);
                      async_read_body();
                    }
                    else
                    {
                      add_incoming_packet();
                    }
                  }
                  else
                  {
                    std::cerr << "[mudpp][network][connection #" << id_
                              << "] Failure reading packet header\n"
                              << ec << "\n";
                    socket_.close();
                  }
                }
                );
    }

    void async_read_body()
    {
      boost::asio::
      async_read( socket_
                , boost::asio::buffer ( buffer_.body.data()
                                      , buffer_.body.size()
                                      )
                , [this](auto ec, std::size_t)
                {
                  if(!ec)
                  {
                    add_incoming_packet();
                  }
                  else
                  {
                    std::cerr << "[mudpp][network][connection #" << id_
                              << "] Failure " << ec << "reading packet body\n";
                    socket_.close();
                  }
                }
                );
    }

    void async_write_header()
    {
      boost::asio::
      async_write( socket_
                , boost::asio::buffer ( &outgoing_.front()
                                      , sizeof( packet_header<T> )
                                      )
                , [this](auto ec, std::size_t)
                {
                  if(!ec)
                  {
                    if(outgoing_.front().body.size() > 0)
                    {
                      async_write_body();
                    }
                    else
                    {
                      outgoing_.pop_front();
                      if(!outgoing_.empty()) async_write_header();
                    }
                  }
                  else
                  {
                    std::cerr << "[mudpp][network][connection #" << id_
                              << "] Failure writing packet header\n";
                    socket_.close();
                  }
                }
                );
    }

    void async_write_body()
    {
      boost::asio::
      async_write ( socket_
                  , boost::asio::buffer ( outgoing_.front().body.data()
                                        , outgoing_.front().body.size()
                                        )
                ,   [this](auto ec, std::size_t)
                {
                  if(!ec)
                  {
                    outgoing_.pop_front();
                    if(!outgoing_.empty()) async_write_header();
                  }
                  else
                  {
                    std::cerr << "[mudpp][network][connection #" << id_
                              << "] Failure writing packet body\n";
                    socket_.close();
                  }
                }
                );
    }

    void add_incoming_packet()
    {
      if(ownership_ == owner::server)
      {
        incoming_.push_back({this->shared_from_this(), buffer_});
      }
      else
      {
        // Conenction has only one client
        incoming_.push_back({nullptr, buffer_});
      }

      async_read_header();
    }

    protected:
    boost::asio::io_context&      context_;
    boost::asio::ip::tcp::socket  socket_;
    safe_queue<packet<T>>         outgoing_;
    safe_queue<message<T>>&       incoming_;
    packet<T>                     buffer_;
    owner                         ownership_ = owner::server;
    std::uint32_t                 id_;
  };
}
