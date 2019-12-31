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

#include <boost/asio.hpp>

namespace mudpp
{
  struct periodic_event
  {
    periodic_event(boost::asio::io_service& ios)
                  : timer(ios, boost::asio::chrono::milliseconds(1))
    {}

    template<typename F, typename Duration>
    void setup(F callback, Duration d)
    {
      auto call = [this,d,callback](auto const& e, auto const& self)
      {
        callback();

        timer.expires_at(timer.expiry() + d);
        timer.async_wait( [this,self](auto const& e) { self(e,self); }) ;
      };

      timer.async_wait( [this,call](auto const& e) { call(e,call); } );
    }

    boost::asio::steady_timer timer;
  };
}

#endif

