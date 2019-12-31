//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_SYSTEM_PERIODIC_EVENT_HPP_INCLUDED
#define MUDPP_SYSTEM_PERIODIC_EVENT_HPP_INCLUDED

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <chrono>
#include <memory>

namespace mudpp
{
  struct periodic_event
  {
    template<typename EventCallback>
    periodic_event( boost::asio::io_service& ios, int p, EventCallback callback )
                  : timer(ios,std::chrono::milliseconds(p))
    {
      auto period = std::chrono::milliseconds(p);

      // We use the 'self-recursive lambda' tricks to pass the lambda while reloading the timer
      auto call = [this,period,callback](auto const& e, auto const& self)
      {
        callback();
        timer.expires_at(timer.expiry() + period);
        timer.async_wait( [this,self](auto const& e) { self(e,self); }) ;
      };

      timer.async_wait( [this,call](auto const& e) { call(e,call); } );
    }

    template<typename EventCallback>
    static auto  make( boost::asio::io_service& ios, int period, EventCallback callback )
    {
      return std::make_unique<periodic_event>(ios,period,callback);
    }

    private:
    boost::asio::steady_timer timer;
  };

  // periodic_event requires dynamic allocation for ease of use, hence the unique_ptr wrapper
  using periodic_event_t = std::unique_ptr<periodic_event>;
}

#endif

