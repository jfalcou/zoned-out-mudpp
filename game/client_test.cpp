//==================================================================================================
/**
  MUPP - MUD in C++
  Copyright 2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/system/network/client.hpp>
#include <chrono>

enum class messages { accept, deny, ping, broadcast, info };

namespace mnet = mudpp::system::network;

class test_client : public mnet::client<messages>
{
  public:
  void ping()
  {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    auto msg = mnet::make_packet(messages::ping, now);
    send(msg);
  }
};

int main(int, char**)
{
  test_client client_instance;
  client_instance.connect("127.0.0.1",60000);

  if(client_instance.is_connected())
  {
    client_instance.ping();
    bool done = false;

    while(!done)
    {
      while(!client_instance.incoming().empty())
      {
        auto now = std::chrono::high_resolution_clock::now();
        auto msg = client_instance.incoming().pop_front().payload;

        switch(msg.header.id)
        {
          case messages::ping:
          {
            decltype(now) then;
            msg >> then;

            std::cout << "Ping: " << std::chrono::duration<double>(now - then).count() << "\n";
            done = true;
          }
          break;

          case messages::accept:
          {
            std::cout << "Connection accepted !\n";
          }
          break;

          default:break;
        }
      }
    }
  }
  else
  {
    std::cout << "Server is down\n";
  }

  return 0;
}
