//==================================================================================================
/**
  MUPP - MUD in C++
  Copyright 2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/system/network/server.hpp>

enum class messages { accept, deny, ping, broadcast, info };

namespace mnet = mudpp::system::network;

class test_server : public mnet::server<messages>
{
  public:

  using parent = mnet::server<messages>;

  test_server( std::uint16_t port) : parent(port)
  {

  }

  protected:
  bool on_connect(std::shared_ptr<mnet::connection<messages>> client) override
  {
    mnet::packet<messages> msg = mnet::make_packet(messages::accept);
    client->send(msg);
    return true;
  }

  void on_disconnect(std::shared_ptr<mnet::connection<messages>> client) override
  {
    std::cout << "client " << client->id() << " disconnected\n";
  }

  void on_message ( std::shared_ptr<mnet::connection<messages>> client
                  , mnet::packet<messages> const& msg
                  ) override
  {
    switch(msg.header.id)
    {
      case messages::ping:
      {
        std::cout << "Ping from " << client->id() << std::endl;
        client->send(msg);
      }
      break;
      default:break;
    }
  }
};

int main(int, char**)
{
  test_server server_instance(60000);
  server_instance.start();

  while(1)
  {
    server_instance.update();
  }

  return 0;
}
