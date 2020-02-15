//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_GAME_HPP_INCLUDED
#define MUDPP_ENGINE_GAME_HPP_INCLUDED

#include <mudpp/system/session_manager.hpp>
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/room.hpp>
#include <mudpp/engine/zone.hpp>
#include <mudpp/deps/api.hpp>
#include <boost/asio.hpp>
#include <sol/sol.hpp>
#include <string_view>
#include <functional>
#include <vector>
#include <map>

namespace mudpp
{
  struct session;

  struct MUDPP_API game
  {
    game( std::string const& config_file );

    std::ostream& log(std::ostream& os, std::string const& context);

    bool run();
    void shutdown();

    void broadcast(std::string const& message);

    room*   find_room(int id);

    auto const& players() const { return players_; }
    player* find_player(std::string const& name);
    player* attach_player(session&);

    std::map<std::string,std::string>&      strings()       { return messages_.value(); }
    sol::state&                             script_engine() { return lua_state_;        }
    boost::asio::io_service&                io()            { return ios_;              }
    std::map<std::string,std::string>&      paths()         { return paths_.value();    }

    template<typename Ret, typename Name, typename... Args>
    auto inline call(Name const& f, Args&&... args)
    {
      sol::function func = lua_state_[f];
      if constexpr( !std::is_same_v<Ret,void> )
      {
        Ret that = func(std::forward<Args>(args)...);
        return that;
      }
      else
      {
        func(std::forward<Args>(args)...);
      }
    }

    bool exists( std::string const& name);

    template<typename Function> void register_event(int period, Function f)
    {
      events_.push_back( periodic_event::make(ios_, period, f) );
    }

    private:
    void load_zones();

    // LUA
    void setup_scripting( std::string const& config_file );
    sol::state                                      lua_state_;
    sol::usertype<player>                           player_type_;
    sol::usertype<room>                             room_type_;
    sol::usertype<zone>                             zone_type_;
    sol::table                                      system_module_;

    // NETWORK
    boost::asio::io_service                         ios_;
    std::vector<periodic_event_t>                   events_;
    std::vector<player_t>                           players_;
    sol::nested<std::map<std::string, std::string>> paths_;
    sol::nested<std::map<std::string, std::string>> messages_;
    std::map<int, zone>                             zones_;
    std::unique_ptr<mudpp::session_manager>         sessions_;

    // GAME
    void cleanup();
    int                                             period_;
    bool                                            shutdown_;
  };

}

#endif
