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
#include <mudpp/engine/state/login.hpp>
#include <mudpp/system/session.hpp>
#include <mudpp/system/io.hpp>
#include <sol/sol.hpp>
#include <fstream>

namespace mudpp
{
  using namespace std::literals;

  player_t player::make(session& s) { return std::make_unique<player>(s); }

  player::player(session& s)
        : session_(s)
        , game_context_(s.context())
        , current_state_("login")
        , current_room_(0)
        , current_game_state_()
        , logged_(false)
  {}


  void player::login()                            { current_game_state_ = login_state::make(this);  }
  void player::enqueue(std::string const& input)  { commands_.push( command::make(input) );         }
  void player::disconnect()                       { session_.disconnect(); logged_ = false;         }
  void player::shutdown()                         { game_context_.shutdown();                       }
  void player::prompt()                           { this->send("#b@y>## ");                         }
  void player::exit()                             { if(current_room_) current_room_->on_exit(this); }
  void player::send( std::string const& msg )     { session_.send(colorize(msg));                   }

  void player::process_input(std::string const& input)
  {
    auto next_state = current_game_state_->process_input(input);
    if(next_state != current_game_state_.get()) current_game_state_.reset(next_state);
  }

  void player::tick()
  {
    if(!commands_.empty())
    {
      current_game_state_->execute(*commands_.front());
      commands_.pop();
    }
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

  void player::go(int direction)
  {
    // Find room in given direction
    if(current_room_)
    {
      int dest = current_room_->traverse(direction,this);
      if(dest >= 0)
      {
        this->exit();
        enter(dest);
      }
      else
      {
        if(dest==-2) send("Where are you going ?\n");
      }
    }
  }

  void player::save(std::string const& data)
  {
    auto path = game_context_.paths()["saves"]  + name_ + ".player";
    game_context_.log(std::cout,"PLAYER") << "Saving character to: "sv  << path << std::endl;
    std::ofstream file( path.c_str() );

    file << "--\n-- MUDPP SAVE FILE\n--\n"sv;
    file << "player =  { name      = \""sv << name_     << "\",\n"sv;
    file << "            password  = \""sv << password_ << "\",\n"sv;
    file << "            "sv << data;
    file << "          }\n"sv;
  }

  void player::setup_scripting( sol::usertype<player>& player_type, sol::state& lua)
  {
    // make usertype metatable
    player_type = lua.new_usertype<player>( "player"
                                          , sol::meta_function::index     , &player::get
                                          , sol::meta_function::new_index , &player::set
                                          , sol::meta_function::length    , &player::length
                                          );

    player_type["send"]         = &player::send;
    player_type["save"]         = &player::save;
    player_type["enter"]        = &player::enter;
    player_type["exit"]         = &player::exit;
    player_type["go"]           = &player::go;
    player_type["location"]     = &player::location;
    player_type["name"]         = sol::property(&player::name, &player::set_name);
    player_type["password"]     = sol::property(&player::password, &player::set_password);
  }
}
