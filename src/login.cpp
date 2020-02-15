//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/state/login.hpp>
#include <mudpp/engine/state/load.hpp>
#include <mudpp/engine/state/create.hpp>
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/game.hpp>
#include <mudpp/system/io.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <memory>

namespace mudpp
{
  login_state::login_state(player* p) : current_player_(p), current_state_(login_states::select_)
  {
    current_player_->context().log(std::cout,"LOGIN") << "Player attempt login" << std::endl;

    auto b = box_message( {"@", tabulate::Color::yellow}
                        , {"¤", tabulate::Color::yellow}
                        , { {tabulate::FontStyle::bold}
                          , tabulate::Color::red
                          , tabulate::FontAlign::center
                          }
                        , 100
                        , p->context().strings()["welcome"]
                        );

    current_player_->send(b);
    prompt();
  }

  void login_state::prompt() const
  {
    auto msg  = colorize( "Would you want to "
                        + display_command("Load") + " or " + display_command("Create")
                        + " a new character ?"
                        );

    current_player_->send(msg);
  }

  std::unique_ptr<login_state> login_state::make(player* p)
  {
    return std::make_unique<login_state>(p);
  }

  game_state* login_state::process_input(std::string const& input)
  {
    if(boost::iequals(input,"L") || boost::iequals(input,"LOAD") )
    {
      current_state_ = login_states::load_;
      return new load_state(current_player_);
    }
    else if(boost::iequals(input,"C") || boost::iequals(input,"CREATE") )
    {
      current_state_ = login_states::create_;
      return new create_state(current_player_);
    }
    else
    {
      current_player_->send("##Sorry, your choice is invalid.\n\r");
      prompt();
      return this;
    }
  }
}
