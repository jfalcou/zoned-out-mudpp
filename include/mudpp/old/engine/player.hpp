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
#include <mudpp/engine/game_state.hpp>
#include <mudpp/engine/command.hpp>
#include <sol/sol.hpp>
#include <string>
#include <memory>
#include <queue>

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
    void login();
    void save(std::string const& data);
    void process_input(std::string const& input);
    void send(std::string const& msg);

    void go(int direction);
    void prompt();

    void enter(int id);
    void exit();

    void enqueue(std::string const& input);

    std::size_t length() const                                { return properties_.length();  }
    void set(std::string const& key, sol::stack_object value) { properties_.set(key,value);   }
    sol::object get(std::string const& key) const             { return properties_.get(key);  }

    room const&         location() const { return *current_room_; }
    std::string const&  name() const { return name_; }
    void                set_name(std::string const& n) { name_ = n; }
    std::string const&  password() const { return password_; }
    void                set_password(std::string const& p) { password_ = p; }

    std::string const&   current_state()  const { return current_state_;  }
    void  set_state(std::string s)              { current_state_ = s;     }

    bool is_connected() const
    {
      using namespace std::literals;
      return current_state_ != "disconnected"sv;
    }

    void set_logged()       { logged_ = true; }
    bool is_logged() const  { return logged_; }

    game&  context() { return game_context_; }

    static void setup_scripting( sol::usertype<player>& player_type, sol::state& );

    private:
    property_map                properties_;
    session&                    session_;
    game&                       game_context_;
    std::string                 current_state_;
    std::string                 name_, password_;
    room*                       current_room_;
    std::unique_ptr<game_state> current_game_state_;
    std::queue<command_t>       commands_;
    bool                        logged_;
  };

  using player_t = std::unique_ptr<player>;
}

#endif
