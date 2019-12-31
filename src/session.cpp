#include <mudpp/system/session_manager.hpp>
#include <mudpp/system/session.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace mudpp
{
  session::session(boost::asio::io_service& io, session_manager* parent)
          : manager_(parent), ios_(io), socket_(io), connection_status_(true)
  {
  }

  void session::start()
  {
    socket_.async_read_some ( boost::asio::buffer(buffer_.data(), buffer_.size())
                            , [this](auto const& e, auto sz) { handle_read(e,sz); }
                            );
  }

  void session::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred)
  {
      if (!error && is_valid())
      {
        std::string cmd(buffer_.begin(),buffer_.begin()+bytes_transferred);
        boost::erase_all(cmd, "\r\n");

        if(cmd == "/quit")
        {
          std::cout << "[INFO] - Connection #" << socket_.native_handle() << " disconnected.\n";
          connection_status_ = false;
        }
        else if(cmd == "/shutdown")
        {
          std::cout << "[SHUTDOWN]\n";
          ios_.stop();
        }
        else
        {
          boost::asio::async_write(socket_,
              boost::asio::buffer(buffer_.data(), bytes_transferred),
              boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
        }
      }
      else
      {
        if ((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error))
        {
          std::cout << "[INFO] - Connection #" << socket_.native_handle() << " disconnected.\n";
          connection_status_ = false;
        }
      }
  }

  void session::handle_write(const boost::system::error_code& error)
  {
    if (!error && is_valid())
    {
      socket_.async_read_some ( boost::asio::buffer(buffer_.data(), buffer_.size())
                              , [this](auto const& e, auto sz) { handle_read(e,sz); }
                              );
    }
    else
    {
      if ((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error))
      {
        std::cout << "[INFO] - Connection #" << socket_.native_handle() << " disconnected.\n";
        connection_status_ = false;
      }
    }
  }
}
