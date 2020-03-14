//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_STATE_ACTION_HPP_INCLUDED
#define MUDPP_ENGINE_STATE_ACTION_HPP_INCLUDED
#include <mudpp/engine/player.hpp>
#include <string>
#include <vector>

namespace mudpp
{
  struct player;

  namespace detail
  {
    void transport(player*, std::vector<std::string> const&);
    void who      (player*, std::vector<std::string> const&);
    void emote    (player*, std::vector<std::string> const&);
    void go       (player*, std::vector<std::string> const&);
    void look     (player*, std::vector<std::string> const&);
    void open     (player*, std::vector<std::string> const&);
    void quit     (player*, std::vector<std::string> const&);
    void say      (player*, std::vector<std::string> const&);
    void shutdown (player*, std::vector<std::string> const&);
    void tell     (player*, std::vector<std::string> const&);
  }
}

#endif
