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
#include <iostream>
#include <memory>

namespace mudpp
{
  create_state::create_state(player* p) : current_player_(p), current_step_(0)
  {
    current_player_->context().call<void>( "creation_menu", current_player_, current_step_);
  }

  std::unique_ptr<create_state> create_state::make(player* p)
  {
    return std::make_unique<create_state>(p);
  }

  game_state* create_state::process_input(std::string const& input)
  {
    int max_step  = current_player_->context().call<int>("creation_max_step");
    auto& ctx     = current_player_->context();

    if(input.empty())
    {
      ctx.call<void>("creation_menu", current_player_, current_step_);
    }
    else
    {
      current_step_ = ctx.call<int>( "creation_handler", current_player_, current_step_, input);

      if(current_step_ < max_step)
        ctx.call<void>("creation_menu", current_player_, current_step_);
    }

    if(current_step_ == max_step)
    {
      ctx.call<void>("save_player", current_player_);
      current_player_->send( ctx.strings()["new_player"]);
      current_player_->enqueue( "%transport 0" );
      current_player_->set_logged();

      return new play_state(current_player_);
    }
    else
    {
      return this;
    }
  }
}
