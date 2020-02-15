//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/state/action.hpp>
#include <mudpp/engine/state/play.hpp>
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/game.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace mudpp
{
  play_state::play_state(player* p) : current_player_(p)
  {
    current_player_->context()
                    .log(std::cout,"LOGIN") << "Player '" << current_player_->name() << "' : play"
                                            << std::endl;
  }

  std::unique_ptr<play_state> play_state::make(player* p)
  {
    return std::make_unique<play_state>(p);
  }

  game_state* play_state::process_input(std::string const& input)
  {
    auto data = input;
    boost::trim_if(data, boost::is_any_of(" "));

    if( !boost::starts_with(data, "%") )
      current_player_->enqueue( data );
    else
      current_player_->prompt();

    return this;
  }

  void play_state::execute(command const& c)
  {
    auto action = c.action();
    boost::to_lower(action);

    auto it = base_commands_.find(action);
    if(it != base_commands_.end())
    {
      it->second(current_player_,c.arguments());
    }
    else
    {
      current_player_->send("What do you mean by '#b@y" + action + "##' ?\n\r");
    }

    current_player_->prompt();
  }
}

//----------------------------------------------------------------------------------------------
// Command registrations
// Add new command->functions here
//----------------------------------------------------------------------------------------------
namespace mudpp
{
  using namespace std::literals;

  std::map<std::string, play_state::command_handle> play_state::base_commands_ =
  {
    //----------------------------------------------------------------------------------------------
    // Register meta-command
    //----------------------------------------------------------------------------------------------
    {"/shutdown"  , ::mudpp::detail::shutdown   }
  , {"/quit"      , ::mudpp::detail::quit       }
  , {"/who"       , ::mudpp::detail::who        }
    //----------------------------------------------------------------------------------------------
    // Register internal commands
    //----------------------------------------------------------------------------------------------
  , {"%transport" , ::mudpp::detail::transport  }
    //----------------------------------------------------------------------------------------------
    // Register action
    //----------------------------------------------------------------------------------------------
  , {"/me"      , ::mudpp::detail::emote  }
  , {"say"      , ::mudpp::detail::say    }
  , {"tell"     , ::mudpp::detail::tell   }
  , {"look"     , ::mudpp::detail::look   }
  , {"go"       , ::mudpp::detail::go     }
    // All directions and directions shortcut
  , {"n"        , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"n"s    }); } }
  , {"north"    , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"north"s}); } }
  , {"s"        , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"s"s    }); } }
  , {"south"    , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"south"s}); } }
  , {"e"        , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"e"s    }); } }
  , {"east"     , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"east"s }); } }
  , {"w"        , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"w"s    }); } }
  , {"west"     , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"west"s }); } }
  , {"u"        , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"u"s    }); } }
  , {"up"       , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"up"s   }); } }
  , {"d"        , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"d"s    }); } }
  , {"down"     , [](auto* p, auto const&) { ::mudpp::detail::go(p,{"down"s }); } }
  };
}
