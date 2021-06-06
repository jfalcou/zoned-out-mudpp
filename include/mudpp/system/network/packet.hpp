//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright : MudPP Contributors & Maintainers
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <vector>
#include <type_traits>

namespace mudpp::system::network
{
  //================================================================================================
  // header for a network packet: [packet ID][packet Size]
  //================================================================================================
  template<typename T> struct packet_header
  {
    T             id    = {};
    std::uint32_t size  = 0UL;
  };

  //================================================================================================
  // A packet is a header + a container of bytes
  //================================================================================================
  template<typename T> struct packet
  {
    packet_header<T> header = {};
    std::vector<std::byte> body;

    std::size_t size() const { return body.size(); }

    friend std::ostream& operator << (std::ostream& os, const packet<T>& msg)
    {
      os << "ID:" << static_cast<int>(msg.header.id) << " Size:" << msg.header.size;
      return os;
    }

    //==============================================================================================
    // Add a POD value to the packet
    //==============================================================================================
    template<typename Type> friend packet<T>& operator<< (packet<T>& msg, const Type& data)
    {
      static_assert ( std::is_standard_layout_v<Type>
                    , "[MUDPP][SYSTEM][NETWORK] packet only accepts POD like entity"
                    );

      auto data_position = msg.body.size();
      msg.body.resize(msg.body.size() + sizeof(Type));
      std::memcpy(msg.body.data() + data_position, (std::byte*)(&data), sizeof(Type));
      msg.header.size = msg.size();

      return msg;
    }

    //==============================================================================================
    // Extracts a POD value from a packet
    //==============================================================================================
    template<typename Type> friend packet<T>& operator>> (packet<T>& msg, Type& data)
    {
      static_assert ( std::is_standard_layout_v<Type>
                    , "[MUDPP][SYSTEM][NETWORK] packet only accepts POD like entity"
                    );

      auto data_position = msg.body.size() - sizeof(Type);

      std::memcpy((std::byte*)(&data), msg.body.data() + data_position, sizeof(Type));
      msg.body.resize(data_position);
      msg.header.size = msg.size();
      return msg;
    }

    // TODO : make an interface based on consuming tuples
    // TODO : make tuple based packet no-alloc + structured bindable
  };

  //================================================================================================
  // Helper functions to create a packet from a bunch of data
  //================================================================================================
  template<typename T, typename... Ts> packet<T> make_packet(T header, Ts... values)
  {
    packet<T> that;
    that.header.id = header;
    ((that << values), ...);
    return that;
  }
}
