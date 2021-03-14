//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_STATE_LOAD_HPP_INCLUDED
#define MUDPP_ENGINE_STATE_LOAD_HPP_INCLUDED

#include <mudpp/engine/game_state.hpp>
#include <memory>

namespace mudpp
{
  struct player;

  struct load_state final : public game_state
  {
    load_state(player* p);

    static std::unique_ptr<load_state> make(player* p);

    virtual game_state* process_input(std::string const& input);

    private:
    void prompt() const;
    player*       current_player_;
  };
}

#endif
