//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/command.hpp>
#include <mudpp/engine/player.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

namespace mudpp
{
  std::unique_ptr<command> command::make(std::string const& d)
  {
    return std::make_unique<command>(d);
  }

  command::command(std::string const& data) : command_{}, arguments_{}
  {
    if(!data.empty())
    {
      std::vector<std::string> tokens;

      boost::split( tokens, data
                  , boost::is_any_of(" \t")
                  , boost::token_compress_on
                  );

      command_ = std::move(tokens.front());
      boost::to_upper(command_);

      if(tokens.size() > 1)
      {
        std::move(tokens.begin()+1, tokens.end(), std::back_inserter(arguments_));
      }
    }
  }
}
