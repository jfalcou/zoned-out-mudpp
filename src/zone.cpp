//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/game.hpp>
#include <mudpp/engine/zone.hpp>
#include <mudpp/engine/room.hpp>

namespace mudpp
{
  zone::zone(game& g) : game_context_(g) {}

  zone::zone(game& g, int id, sol::table const& data)
      : game_context_(g)
      , name_(data["name"]), description_(data["desc"])
      , id_(id)
      , start_room_(data["start"])
  {
    // Retrieve LUA list of rooms
    sol::table rooms = data["rooms"];

    // Build all rooms from current zone
    for (auto it : rooms)
    {
      int        r_id = it.first.as<int>();
      sol::table r    = it.second.as<sol::table>();
      rooms_.insert({r_id, room(game_context_,r_id,r)});
    }
  }

  void zone::tick()
  {
    // If zone is empty of player and that's x mn, just reset it

    // Update all active room in zone

    // Other upkeep
  }

  room* zone::find_room(int id)
  {
    auto r = rooms_.find(id);
    if(r != rooms_.end()) return &(r->second);
    else                  return nullptr;
  }

  void zone::setup_scripting( sol::usertype<zone>& ut, sol::state& lua)
  {
    // make usertype metatable
    ut = lua.new_usertype<zone>("zone");
    ut["tick"]         = &zone::tick;
    ut["id"]           = sol::property(&zone::id          , &zone::set_id);
    ut["name"]         = sol::property(&zone::name        , &zone::set_name);
    ut["description"]  = sol::property(&zone::description , &zone::set_description);
  }
}
