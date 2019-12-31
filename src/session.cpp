#include <mudpp/system/session_manager.hpp>
#include <mudpp/system/session.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace mudpp
{
  session::session(boost::asio::io_service& io)
          : ios_(io), socket_(io), connection_status_(true)
  {
  }

  void session::start()
  {
    socket_.async_read_some ( boost::asio::buffer(data, max_length)
                            , [this](auto const& e, auto sz) { handle_read(e,sz); }
                            );
  }

  void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
  {
      if (!error && is_valid())
      {
        std::string cmd(data,data+bytes_transferred-2);

        std::cout << "session::handle_read " << (int) bytes_transferred << " bytes\t"
        << cmd << "\n";

        if(cmd == "/quit")
        {
          std::cout << "CONNECTION QUITTING : " << socket_.native_handle() << "\n";
          connection_status_ = false;
        }
        if(cmd == "/reboot")
        {
          std::cout << "SERVER QUITTING : \n";
          ios_.stop();
        }
        else
        {
          boost::asio::async_write(socket_,
              boost::asio::buffer(data, bytes_transferred),
              boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
        }
      }
      else
      {
        if ((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error))
        {
          std::cout << "CONNECTION CUT OFF : " << socket_.native_handle() << "\n";
          connection_status_ = false;
        }
      }
  }

  void session::handle_write(const boost::system::error_code& error)
  {
    if (!error && is_valid())
    {
      std::cout << "session::handle_write\n";
      socket_.async_read_some ( boost::asio::buffer(data, max_length)
                              , [this](auto const& e, auto sz) { handle_read(e,sz); }
                              );
    }
    else
    {
      std::cout << "session::handle_write error: " << error << std::endl;
      if ((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error))
      {
        std::cout << "CONNECTION CUT OFF BY CLIENT\n";
        connection_status_ = false;
      }
    }
  }
}
