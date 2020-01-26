//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_ZONE_HPP_INCLUDED
#define MUDPP_ENGINE_ZONE_HPP_INCLUDED

#include <mudpp/engine/room.hpp>
#include <sol/sol.hpp>
#include <string>
#include <map>

namespace mudpp
{
  struct game;

  struct zone
  {
    zone(game& g);
    zone(game& g, int id, sol::table const& data);

    static void setup_scripting( sol::usertype<zone>&, sol::state& );

    void tick();
    room* find_room(int id);

    int                 id()          const noexcept { return id_; }
    std::string const&  name()        const noexcept { return name_; }
    std::string const&  description() const noexcept { return description_; }

    void set_id(int id)                         { id_ = id; }
    void set_name(std::string const& n)         { name_ = n; }
    void set_description(std::string const& d)  { description_ = d; }

    private:
    game&                         game_context_;
    std::map<int,room>            rooms_;
    std::string                   name_;
    std::string                   description_;
    int                           id_;
    int                           start_room_;
  };
}

#endif
