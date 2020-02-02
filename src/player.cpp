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
  using namespace std::literals;

  void player::setup_scripting( sol::usertype<player>& player_type, sol::state& lua)
  {
    // make usertype metatable
    player_type = lua.new_usertype<player>( "player"
                                          , sol::meta_function::index     , &player::get
                                          , sol::meta_function::new_index , &player::set
                                          , sol::meta_function::length    , &player::length
                                          );

    player_type["tick"]         = &player::tick;
    player_type["shutdown"]     = &player::shutdown;
    player_type["disconnect"]   = &player::disconnect;
    player_type["send"]         = &player::send;
    player_type["save"]         = &player::save;
    player_type["enter"]        = &player::enter;
    player_type["exit"]         = &player::exit;
    player_type["go"]           = &player::go;
    player_type["name"]         = sol::property(&player::name, &player::set_name);
    player_type["password"]     = sol::property(&player::password, &player::set_password);
    player_type["prompt"]       = &player::login_prompt;
    player_type["is_connected"] = &player::is_connected;
    player_type["is_logged"]    = &player::is_logged;
    player_type["location"]     = &player::location;
    player_type["state"]        = sol::property(&player::current_state, &player::set_state);
  }

  player::player(session& s)
        : session_(s)
        , game_context_(s.context())
        , current_state_(0)
        , current_room_(0)
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
    current_state_ = game_context_.call<int>("process_input"sv, current_state_, *this, input);
  }

  void player::disconnect() { session_.disconnect(); }
  void player::shutdown()   { game_context_.shutdown();     }

  void player::send( std::string const& msg, bool use_color )
  {
    if(use_color) session_.send(colorize(msg));
    else          session_.send(msg);
    if(current_state_ == 50) session_.send(colorize("#b>##"));
  }

  void player::tick()
  {
    if(current_state_ == 50)
      send(info("\n\r**TICK**"sv));
  }

  void player::enter(int id)
  {
    if(auto r = game_context_.find_room(id))
    {
      current_room_ = r;
      current_room_->on_enter(this);
    }
    else
    {
      game_context_.log(std::cerr,"PLAYER") << name_ << " tried to access room #"sv << id
                                            << " which is not registered."sv;
      send("Where are you going ?\n");
    }
  }

  void player::exit()
  {
    if(current_room_)
    {
      current_room_->on_exit(this);
    }
  }

  void player::go(int direction)
  {
    // Find room in given direction
    if(current_room_)
    {
      int dest = current_room_->go(direction);
      if(dest != -1)
      {
        this->exit();
        enter(dest);
      }
      else
      {
        send("Where are you going ?\n");
      }
    }
  }

  void player::save(std::string const& data)
  {
    auto path = game_context_.paths()["saves"]  + name_ + ".player";
    game_context_.log(std::cout,"PLAYER") << "Saving character to: "sv  << path << std::endl;
    std::ofstream file( path.c_str() );

    file << "--\n"sv;
    file << "-- MUDPP SAVE FILE\n"sv;
    file << "--\n"sv;
    file << "player =  { name      = \""sv << name_     << "\",\n"sv;
    file << "            password  = \""sv << password_ << "\",\n"sv;
    file << "            "sv << data;
    file << "          }\n"sv;
  }
}
