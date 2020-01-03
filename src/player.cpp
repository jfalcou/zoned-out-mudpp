//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/engine/player.hpp>
#include <mudpp/engine/game.hpp>
#include <mudpp/system/session.hpp>
#include <mudpp/system/io.hpp>
#include <tabulate/termcolor.hpp>
#include <tabulate/table.hpp>

namespace mudpp
{
  player::player(session& s)
        : session_(s)
        , game_context_(s.context())
        , current_state_(state::login)
  {
    banner();
    std::ostringstream os;
    os  << termcolor::colorize << termcolor::white << "Would you want to "
        << termcolor::bold << "[" << termcolor::yellow << "L" << termcolor::white << "]oad"
        << termcolor::reset << " or "
        << termcolor::bold << "[" << termcolor::yellow << "C" << termcolor::white << "]reate"
        << termcolor::reset << " a new character ?"
        << "\n";

    session_.send(os.str());
  }

  player_t player::make(session& s)
  {
    return std::make_unique<player>(s);
  }

  void player::process_input(std::string const& input)
  {
    switch(current_state_)
    {
      case state::login:  current_state_ = process_login(input);
                          break;

      case state::play:   current_state_ = play(input);
                          break;

      case state::new_player: current_state_ = create_player(input);
                              break;

      case state::disconnected: break;

      default:  break;
    }
  }

  void player::send( std::string const& msg ) { session_.send(msg); }

  void player::tick() { if(current_state_ == state::play) send(info("**TICK**")); }

  // Display the banner to current player
  void player::banner()
  {
    tabulate::Table banner_;

    banner_ .format()
            .width(80)
            .multi_byte_characters(true)
            .font_style({tabulate::FontStyle::bold})
            .font_color(tabulate::Color::red)
            .font_align(tabulate::FontAlign::center)
            .corner("ᛰ")
            .corner_color(tabulate::Color::yellow)
            .border("ᛟ")
            .border_color(tabulate::Color::yellow);

    banner_.add_row({" "});
    banner_.add_row({"Welcome to Mud++"});
    banner_[1].format().hide_border_top();
    banner_.add_row({" "});
    banner_[2].format().hide_border_top();

    std::ostringstream os;
    os << termcolor::colorize << banner_ << "\n";

    send(os.str());
  }

  state player::create_player(std::string const& input)
  {
    if(input.empty()) return state::new_player;

    name_ = input;
    send("Welcome " + input + " !\n");

    return state::play;
  }

  state player::process_login(std::string const& input)
  {
    game_context_.log(std::cout,"GAME") << "Login" << std::endl;

    // Input contains (L or l) / (C or c)
    if(input == "L" || input == "l")
    {
      game_context_.log(std::cout,"GAME") << "Loading character" << std::endl;
      send("Loading player file...\n");
      return state::play;
    }
    else if(input == "C" || input == "c")
    {
      game_context_.log(std::cout,"GAME") << "Creating character" << std::endl;
      send("Character name:\n");
      return state::new_player;
    }
    else
    {
      send("Sorry, your choice is invalid.\n");
      return state::login;
    }
  }

  state player::play(std::string const& input)
  {
    if(input == "/quit")
    {
      send("Bye " + name_ + " !\n");
      current_state_ = state::disconnected;
      session_.disconnect();
    }
    else if(input == "/shutdown")
    {
      send("Bye " + name_ + " !\nThe server will now shutdown ...\n");
      game_context_.shutdown();
    }
    else if(input == "/hello")
    {
      send("Hello " + name_ + " !\n");
    }
    else
    {
      send("No comprendo senor!\n");
    }

    return state::play;
  }
}
