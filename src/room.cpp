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
#include <iostream>

namespace mudpp
{
  room::room(game& g) : game_context_(g) {}

  room::room(game& g, int id, sol::table const& data)
      : game_context_(g)
      , attendees_{}
      , name_(data["name"]), description_(std::string(data["desc"])+"\n")
      , id_(id)
  {
    sol::table e = data["exit"];

    exits_[0] = e.get_or("n",-1);
    exits_[1] = e.get_or("s",-1);
    exits_[2] = e.get_or("e",-1);
    exits_[3] = e.get_or("w",-1);
    exits_[4] = e.get_or("u",-1);
    exits_[5] = e.get_or("d",-1);
  }

  void room::tick()
  {
    if(!attendees_.empty())
    {
      // If room is empty of player and that's x mn, just reset it

      // Update all active mod in room and adjacent room

      // Other upkeep
    }
  }

  void room::on_enter(player* attendee)
  {
    for(auto&& a : attendees_)
      a->send( "#b@y" + attendee->name() + "## enters the room.\n");

    attendees_.insert(attendee);
    attendee->send(description_);
  }

  void room::on_exit(player* attendee)
  {
    attendees_.erase(attendee);
    for(auto&& a : attendees_)
      a->send( "#b@y" + attendee->name() + "## leaves.\n");
  }

  int room::go(int direction) const
  {
    return exits_[direction];
  }

  void room::setup_scripting( sol::usertype<room>& ut, sol::state& lua)
  {
    // make usertype metatable
    ut = lua.new_usertype<room>("room");
    ut["tick"]        = &room::tick;
    ut["go"]          = &room::go;
    ut["id"]          = sol::property(&room::id          , &room::set_id);
    ut["name"]        = sol::property(&room::name        , &room::set_name);
    ut["description"] = sol::property(&room::description , &room::set_description);
  }
}
