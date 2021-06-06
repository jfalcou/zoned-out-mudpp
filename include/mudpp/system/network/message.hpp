//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright : MudPP Contributors & Maintainers
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#pragma once
#include <mudpp/system/network/packet.hpp>
#include <ostream>
#include <memory>

namespace mudpp::system::network
{
  template<typename T> class connection;

  //================================================================================================
  // A message is a packet tied to a connection.
  // On a server, the owner of a message would be the client.
  // On a client, the owner of a message would be the server.
  //================================================================================================
  template<typename T> struct message
  {
    std::shared_ptr<connection<T>>  remote = nullptr;
    packet<T>                       payload;

    friend std::ostream& operator<<(std::ostream& os, const message<T>& msg)
    {
      os << msg.payload;
      return os;
    }
  };
}
