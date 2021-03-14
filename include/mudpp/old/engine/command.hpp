//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_COMMAND_HPP_INCLUDED
#define MUDPP_ENGINE_COMMAND_HPP_INCLUDED

#include <string>
#include <vector>
#include <memory>

namespace mudpp
{
  struct game;
  struct player;

  struct command
  {
    command(std::string const& data);
    static std::unique_ptr<command> make(std::string const& d);

    void run(player*) const;
    bool is_valid() const { return !command_.empty(); }

    auto& action()    const { return command_;    }
    auto& arguments() const { return arguments_;  }

    private:
    std::string               command_;
    std::vector<std::string>  arguments_;
  };

  using command_t = std::unique_ptr<command>;
}

#endif
