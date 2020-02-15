//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/state/load.hpp>
#include <mudpp/engine/state/play.hpp>
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/game.hpp>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <iostream>
#include <memory>

namespace mudpp
{
  load_state::load_state(player* p) : current_player_(p)
  {
    prompt();
  }

  void load_state::prompt() const
  {
    current_player_->send("#b@yCharacter name:## ");
  }

  std::unique_ptr<load_state> load_state::make(player* p)
  {
    return std::make_unique<load_state>(p);
  }

  game_state* load_state::process_input(std::string const& input)
  {
    auto& ctx = current_player_->context();

    if(!current_player_->is_logged())
    {
      if(ctx.exists(input))
      {
        ctx.log(std::cerr,"LOGIN") << "Player '" << input << "' is already logged." << std::endl;
        current_player_->send("@y#b" + input + "## is already logged in.\n\r");
        prompt();
      }
      else
      {
        current_player_->set_name(input);
        auto savegame_path = ctx.paths()["saves"] + input + ".player";

        if( std::filesystem::exists( savegame_path ) )
        {
          ctx.log(std::cout,"LOGIN") << "Loading '" << savegame_path << "'." << std::endl;
          ctx.call<void>("load_player", current_player_, savegame_path);
          current_player_->send("#b@yPassword:## ");
          current_player_->set_logged();
        }
        else
        {
          ctx.log(std::cerr,"LOGIN") << "Unknown player '" << input << "'." << std::endl;
          current_player_->send ( "@rUnknown player @y" + input + "##\n\r");
          prompt();
        }
      }

      return this;
    }
    else
    {
      if(current_player_->password() != input)
      {
        current_player_->send("@rIncorrect password.##\n\r#b@yPassword:## ");
        return this;
      }
      else
      {
        current_player_->send( ctx.strings()["returning_player"]);
        current_player_->enter( 0 );
        return new play_state(current_player_);
      }
    }
  }
}
