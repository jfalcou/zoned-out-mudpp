//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_GAME_STATE_HPP_INCLUDED
#define MUDPP_ENGINE_GAME_STATE_HPP_INCLUDED

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <map>

namespace mudpp
{
  struct player;
  struct command;

  struct game_state
  {
    using command_handle  = std::function<void(player*, std::vector<std::string> const&)>;
    using commands_map    = std::map<std::string,command_handle>;

    virtual ~game_state() {}
    virtual void execute(command const&) {}
    virtual game_state* process_input(std::string const& input) = 0;
  };

  using game_state_t = std::unique_ptr<game_state>;
}

#endif
