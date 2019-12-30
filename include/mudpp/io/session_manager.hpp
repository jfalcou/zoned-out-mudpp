//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_IO_SESSION_MANAGER_HPP_INCLUDED
#define MUDPP_IO_SESSION_MANAGER_HPP_INCLUDED

#include <mudpp/io/connection.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <vector>

namespace mudpp
{
  struct session_manager
  {
     session_manager(int port);
    ~session_manager();

    void          handle_connections();
    connection_t  process_connection();
    std::size_t   connection_count() const { return connections.size(); }

    private:

    static void bailout (int sig);

    int         control_socket_;
    time_t      last_reception_;
    sockaddr_in sa;
    fd_set in_set, out_set, exc_set;

    std::vector<connection_t> connections;
  };
}

#endif
