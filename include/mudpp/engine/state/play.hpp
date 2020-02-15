//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_STATE_PLAY_HPP_INCLUDED
#define MUDPP_ENGINE_STATE_PLAY_HPP_INCLUDED

#include <mudpp/engine/game_state.hpp>
#include <memory>

namespace mudpp
{
  struct player;

  struct play_state final : public game_state
  {
    using command_handle = std::function<void(player*, std::vector<std::string> const&)>;

    play_state(player* p);

    static std::unique_ptr<play_state> make(player* p);

    virtual game_state* process_input(std::string const& input);
    virtual void execute(command const&);

    // Centralized data accross all play states
    static std::map<std::string, command_handle> base_commands_;

    private:
    player*                               current_player_;

    // TODO: Support per player aliases ?
    // std::map<std::string, command_handle> local_commands_;
  };
}

#endif
