//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/state/play.hpp>
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/game.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <string>

namespace mudpp::detail
{
  //------------------------------------------------------------------------------------------------
  // Utility functions
  //------------------------------------------------------------------------------------------------
  // Translate direction to index
  int direction_from(std::string const& s)
  {
          if(boost::iequals(s,"n") || boost::iequals(s,"north"))  return 0;
    else  if(boost::iequals(s,"s") || boost::iequals(s,"south"))  return 1;
    else  if(boost::iequals(s,"e") || boost::iequals(s,"east" ))  return 2;
    else  if(boost::iequals(s,"w") || boost::iequals(s,"west" ))  return 3;
    else  if(boost::iequals(s,"u") || boost::iequals(s,"up"   ))  return 4;
    else  if(boost::iequals(s,"d") || boost::iequals(s,"down" ))  return 5;
    else  return -1;
  }

  // Aggregate strings in a single one
  template<typename Iterator>
  std::string aggregate(Iterator b, Iterator e)
  {
    // Pre-allocate the whole string
    auto sz = std::accumulate( b, e, 0, [](auto acc, auto const& e) { return acc+e.size(); } );
    std::string msg(sz + (e-b),' ');

    // Copy strings into their proper location
    auto next = msg.begin();
    while(b!=e)
    {
      next = std::copy(b->begin(),b->end(),next);
      *next++ = ' ';
      b++;
    }

    // Clean up the ending
    msg.pop_back();
    return msg;
  }

  //------------------------------------------------------------------------------------------------
  // Say command
  // say <player> <text>: Say <text> to <player> if he's in the room
  //------------------------------------------------------------------------------------------------
  void say(player* p, std::vector<std::string> const& args)
  {
    if(args.size() <= 1)
    {
      p->send("Say what to whom ?\n\r");
      return;
    }

    if(auto target = p->context().find_player(args[0]))
    {
      std::string msg = "#b@y" + p->name() + "## says: '";
      msg += aggregate(args.begin()+1,args.end());
      msg += "##'\n\r";
      target->send(msg);
      return;
    }

    p->send("Say what to whom ?\n\r");
  }

  //------------------------------------------------------------------------------------------------
  // Yell command
  // yell <text>: yell <text> to the room
  //------------------------------------------------------------------------------------------------
  void yell(player* p, std::vector<std::string> const& args)
  {
    auto const& current_room = p->location();

    std::string msg = "#b@y" + p->name() + "## yells '";
    msg += aggregate(args.begin(),args.end());
    msg += "'\n\r";

    p->send("You yells to the room ...\n\r");
    for(auto other : current_room.attendees( ))
    {
      if(other != p) other->send(msg);
    }
  }

  //------------------------------------------------------------------------------------------------
  // Go command
  // go <direction>: go in a given direction
  //------------------------------------------------------------------------------------------------
  void go(player* p, std::vector<std::string> const& args)
  {
    if(args.size() >= 1)
    {
      if(auto direction = direction_from(args[0]); direction != -1)
      {
        p->go(direction);
        return;
      }
    }

    // Nowhere to go
    p->send( "Where do you want to go?\n\r" );
  }

  //------------------------------------------------------------------------------------------------
  // Look command
  // look: look in current room
  // look <direction>: look in a given direction
  //------------------------------------------------------------------------------------------------
  void look(player* p, std::vector<std::string> const& args)
  {
    if(args.empty())
    {
      // No arguments, look at the room
      p->send( p->location().description() );
    }
    else
    {
      if(auto direction = direction_from(args[0]); direction != -1)
      {
        auto next_room = p->location().adjacent(direction);
        if(next_room != -1)
        {
          auto new_room = p->context().find_room(next_room);
          p->send("In this direction, you see " + new_room->name() + "\n\r");
          return;
        }
      }

      // Nothing to look at
      p->send( "There is nothing to look at there.\n\r" );
    }
  }

  //------------------------------------------------------------------------------------------------
  // Shutdown privileged command
  //------------------------------------------------------------------------------------------------
  void shutdown(player* p, std::vector<std::string> const&)
  {
    p->context().broadcast( "@r#bThe server will now shutdown ...##\n\r");
    p->shutdown();
  };

  //------------------------------------------------------------------------------------------------
  // Quit command
  //------------------------------------------------------------------------------------------------
  void quit(player* p, std::vector<std::string> const&)
  {
    p->context().broadcast( "@y#b" + p->name() + "## is leaving the game.\n\r" );
    p->context().call<void>("save_player", p);
    p->disconnect();
  }
}
