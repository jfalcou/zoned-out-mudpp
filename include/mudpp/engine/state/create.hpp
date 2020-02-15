//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_STATE_CREATE_HPP_INCLUDED
#define MUDPP_ENGINE_STATE_CREATE_HPP_INCLUDED

#include <mudpp/engine/game_state.hpp>
#include <memory>

namespace mudpp
{
  struct player;

  struct create_state final : public game_state
  {
    enum class step { name_, password_ };

    create_state(player* p);

    static std::unique_ptr<create_state> make(player* p);

    virtual game_state* process_input(std::string const& input);

    private:
    player*   current_player_;
    step      current_step_;
  };
}

#endif
