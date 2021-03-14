//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/exit.hpp>
#include <iostream>

namespace mudpp
{
  exit::exit() : state_(state::none), target_room_(-2) {}

  exit::exit(sol::table const& data)
      : description_(std::string(data["desc"].get<std::string>()))
      , type_(std::string(data["type"].get<std::string>()))
      , state_(data["state"].get<state>())
      , target_room_(data["target"].get<int>())
  {}

  void exit::lock(player* p)
  {
    if(state_ == state::unlocked)
    {
      state_ = state::locked;
      p->send("The #b@y" + type_ + "## is now locked.\n\r");
    }
    else if(state_ == state::locked)
    {
      p->send("The #b@y" + type_ + "## is already locked.\n\r");
    }
    else
    {
      p->send("You can't do that.\n\r");
    }
  }

  void exit::unlock(player* p)
  {
    if(state_ == state::locked)
    {
      state_ = state::unlocked;
      p->send("The #b@y" + type_ + "## is now unlocked.\n\r");
    }
    else if(state_ == state::unlocked)
    {
      p->send("The #b@y" + type_ + "## is already unlocked.\n\r");
    }
    else
    {
      p->send("You can't do that.\n\r");
    }
  }

  void exit::open(player* p)
  {
    if(state_ == state::closed)
    {
      state_ = state::open;
      p->send("The #b@y" + type_ + "## is now open.\n\r");
    }
    else if(state_ == state::open)
    {
      p->send("The #b@y" + type_ + "## is already open.\n\r");
    }
    else
    {
      p->send("You can't do that.\n\r");
    }
  }

  void exit::close(player* p)
  {
    if(state_ == state::open)
    {
      state_ = state::closed;
      p->send("The #b@y" + type_ + "## is now closed.\n\r");
    }
    else if(state_ == state::closed)
    {
      p->send("The #b@y" + type_ + "## is already closed.\n\r");
    }
    else
    {
      p->send("You can't do that.\n\r");
    }
  }

  std::string exit::describe() const noexcept
  {
    if(state_ == state::open    ) return std::string("open");
    if(state_ == state::closed  ) return std::string("closed");
    if(state_ == state::unlocked) return std::string("unlocked");
    if(state_ == state::locked  ) return std::string("locked");
    return {};
  }

  int exit::traverse(player* p) const
  {
    if(state_ == state::open || state_ == state::direct)
    {
      return target_room_;
    }
    else if(state_ == state::none)
    {
      return -2;
    }
    else
    {
      p->send("You can't do that. The #b@y" + type_ + "## is " + describe() + ".\n\r");
      return -1;
    }
  }

  void exit::tick() {}

  void exit::setup_scripting( sol::usertype<exit>& ut, sol::state& lua)
  {
    // make usertype metatable
    ut = lua.new_usertype<exit>("exit");
    ut["description"] = sol::property(&exit::description , &exit::set_description);

    // Register enum for exit state
    lua.new_enum( "exit_state"
                , "none"  , state::none
                , "open"  , state::open
                , "closed", state::closed
                , "locked", state::locked
                , "direct", state::direct
                );
  }
}
