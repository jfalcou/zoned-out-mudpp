//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/state/create.hpp>
#include <mudpp/engine/state/play.hpp>
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/game.hpp>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <iostream>
#include <memory>

namespace mudpp
{
  create_state::create_state(player* p) : current_player_(p), current_step_(step::name_)
  {
    current_player_->send("#b@yCharacter name:## ");
  }

  std::unique_ptr<create_state> create_state::make(player* p)
  {
    return std::make_unique<create_state>(p);
  }

  game_state* create_state::process_input(std::string const& input)
  {
    auto& ctx = current_player_->context();

    switch(current_step_)
    {
      case step::name_:
      {
        if( std::all_of ( input.begin(), input.end()
                        , [](char c) {  return   (c >= '0' && c <= '9')
                                              || (c >= 'a' && c <= 'z') ||  (c >= 'A' && c <= 'Z')
                                              || (c == '_');
                                      }
                        )
          )
        {
          auto savegame_path = ctx.paths()["saves"] + input + ".player";

          if( std::filesystem::exists( savegame_path ) )
          {
            current_player_->send ( "The name @y" + input + "## is already in use.\n\r"
                                  + "@y#bCharacter name:## "
                                  );
          }
          else
          {
            current_player_->set_name(input);
            ctx.call<void>("setup_player",current_player_);
            current_player_->send( "Welcome @y#b" + input + "## !\n\r");
            current_player_->send( "@y#bChoose a password:## ");
            current_step_ = step::password_;
          }
        }
        else
        {
          current_player_->send ( "The name @y" + input + "## is invalid.\n\r"
                                + "@y#bCharacter name:## "
                                );
        }

        return this;
      }
      break;

      case step::password_:
      {
        if(input.empty())
        {
          current_player_->send( "@y#bChoose a password:## ");
          return this;
        }
        else
        {
          current_player_->set_password(input);
          current_player_->send("Your password is: " + input + "\n\r");
          current_player_->context().call<void>("save_player", current_player_);

          current_player_->send( ctx.strings()["new_player"]);
          current_player_->enqueue( "%transport 0" );
          return new play_state(current_player_);
        }
      }
      break;
    };

    return this;
  }
}
