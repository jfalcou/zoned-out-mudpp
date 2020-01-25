//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/game.hpp>
#include <mudpp/system/session.hpp>
#include <mudpp/system/io.hpp>
#include <sol/sol.hpp>
#include <fstream>

namespace mudpp
{
  void player::setup_scripting( sol::usertype<player>& player_type, sol::state& lua)
  {
    // make usertype metatable
    player_type = lua.new_usertype<player>("player");
    player_type["tick"]         = &player::tick;
    player_type["shutdown"]     = &player::shutdown;
    player_type["disconnect"]   = &player::disconnect;
    player_type["send"]         = &player::send;
    player_type["save"]         = &player::save;
    player_type["name"]         = sol::property(&player::name, &player::set_name);
    player_type["password"]     = sol::property(&player::password, &player::set_password);
    player_type["prompt"]       = &player::login_prompt;
    player_type["is_connected"] = &player::is_connected;
    player_type["is_logged"]    = &player::is_logged;
    player_type["state"]        = sol::property(&player::current_state, &player::set_state);
  }

  player::player(session& s)
        : session_(s)
        , game_context_(s.context())
        , current_state_(0)
  {}

  player_t player::make(session& s) { return std::make_unique<player>(s); }

  void player::login_prompt()
  {
    auto msg  = colorize( "Would you want to "
                        + display_command("Load") + " or "
                        + display_command("Create")
                        + " a new character ?"
                        );

    session_.send(msg);
  }

  void player::process_input(std::string const& input)
  {
    using namespace std::literals;
    current_state_ = game_context_.call<int>("process_input"sv, current_state_, *this, input);
  }

  void player::disconnect()                   { session_.disconnect();        }
  void player::shutdown()                     { game_context_.shutdown();     }

  void player::send( std::string const& msg, bool use_color )
  {
    if(use_color) session_.send(colorize(msg));
    else          session_.send(msg);
  }

  void player::tick() { if(current_state_ == 5) send(info("**TICK**")); }

  void player::save()
  {
    auto path = game_context_.paths()["saves"]  + name_ + ".player";
    game_context_.log(std::cout,"PLAYER") << "Saving character to: "  << path << std::endl;
    std::ofstream file( path.c_str() );

    file << "--\n";
    file << "-- MUDPP SAVE FILE\n";
    file << "--\n";
    file << "player =  { name      = \"" << name_     << "\",\n";
    file << "            password  = \"" << password_ << "\",\n";
    file << "          }\n";
  }
}
