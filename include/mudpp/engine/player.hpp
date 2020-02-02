//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_PLAYER_HPP_INCLUDED
#define MUDPP_ENGINE_PLAYER_HPP_INCLUDED

#include <mudpp/system/property_map.hpp>
#include <mudpp/system/session.hpp>
#include <sol/sol.hpp>
#include <string>
#include <memory>

namespace mudpp
{
  struct game;
  struct room;

  struct player
  {
    player(session& s);
    static std::unique_ptr<player> make(session& s);

    void tick();
    void shutdown();
    void disconnect();
    void login_prompt();
    void save(std::string const& data);
    void process_input(std::string const& input);
    void send( std::string const& msg, bool use_color = true );

    void go(int direction);

    void enter(int id);
    void exit();

    std::size_t length() const                                { return properties_.length();  }
    void set(std::string const& key, sol::stack_object value) { properties_.set(key,value);   }
    sol::object get(std::string const& key) const             { return properties_.get(key);  }

    room const&         location() const { return *current_room_; }
    std::string const&  name() const { return name_; }
    void                set_name(std::string const& n) { name_ = n; }
    std::string const&  password() const { return password_; }
    void                set_password(std::string const& p) { password_ = p; }

    int   current_state()     const { return current_state_;  }
    void  set_state(int s)          { current_state_ = s;     }

    bool is_connected() const { return current_state_ != 60; }
    bool is_logged()    const { return current_state_ == 50; }

    static void setup_scripting( sol::usertype<player>& player_type, sol::state& );

    private:
    session&      session_;
    property_map  properties_;
    game&         game_context_;
    int           current_state_;
    std::string   name_, password_;
    room*         current_room_;
  };

  using player_t = std::unique_ptr<player>;
}

#endif
