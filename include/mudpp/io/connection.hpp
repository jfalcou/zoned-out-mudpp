//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_IO_CONNEXION_HPP_INCLUDED
#define MUDPP_IO_CONNEXION_HPP_INCLUDED

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <memory>
#include <string>

namespace mudpp
{
  struct connection
  {
    connection(int socket, int port, std::string const& address)
              : socket_(socket), port_(port), address_(address), closing_(false)
    {}

    ~connection()
    {
      write();
      if(is_active()) close(socket_);
    }

    void read()           {}
    void write()          {}
    void handle_errors()  {}

    int   socket()      const { return socket_;         }
    bool  is_active()   const { return socket_ != -1;   }
    bool  is_closing()  const { return closing_;        }
    bool  is_pending()  const { return !outbuf.empty(); }

    static auto make(int socket, int port, std::string const& address)
    {
      return std::make_unique<connection>(socket,port,address);
    }

    private:
    int socket_;              // socket they connected on
    int port_;           // port they connected on

    std::string outbuf_;      // pending output
    std::string inbuf_;       // pending input
    std::string address_;     // address player is from

    bool closing_;     // true if they are about to leave us
  };


  // Local using for connection resource pointer
  using connection_t = std::unique_ptr<connection>;
}

#endif
