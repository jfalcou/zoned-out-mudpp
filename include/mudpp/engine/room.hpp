//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_ROOM_HPP_INCLUDED
#define MUDPP_ENGINE_ROOM_HPP_INCLUDED

#include <sol/sol.hpp>
#include <string>
#include <set>
#include <map>

namespace mudpp
{
  struct player;
  struct zone;
  struct game;

  struct room
  {
    room(game& g);
    room(game& g, int id, sol::table const& data);

    static void setup_scripting( sol::usertype<room>&, sol::state& );

    void tick();

    void  on_enter(player*);
    void  on_exit(player*);
    int   go(int direction) const;

    int                       id()          const noexcept { return id_; }
    std::string const&        name()        const noexcept { return name_; }
    std::string const&        description() const noexcept { return description_; }
    std::set<player*> const&  attendees()   const noexcept { return attendees_; }

    void set_id(int id)                         { id_ = id; }
    void set_name(std::string const& n)         { name_ = n; }
    void set_description(std::string const& d)  { description_ = d; }

    private:
    game&                 game_context_;
    std::set<player*>     attendees_;
    std::array<int,6>     exits_;
    std::string           name_;
    std::string           description_;
    int                   id_;
  };
}

#endif

