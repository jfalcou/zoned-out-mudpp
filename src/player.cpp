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
#include <fstream>
#include <filesystem>

namespace mudpp
{
  player::player(session& s)
        : session_(s)
        , game_context_(s.context())
        , current_state_(state::login)
  {
    auto b = box_message( {"@", tabulate::Color::yellow}
                        , {"¤", tabulate::Color::yellow}
                        , {{tabulate::FontStyle::bold},tabulate::Color::red,tabulate::FontAlign::center}
                        , 100
                        , game_context_.strings()["welcome"]
                        //"Welcome to Mud++\n¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤\n\nCopyright 2020 Joel FALCOU"
                        );

    send(b);
    login_prompt();
  }

  player_t player::make(session& s)
  {
    return std::make_unique<player>(s);
  }

  void player::login_prompt()
  {
    std::ostringstream os;
    os  << termcolor::colorize << termcolor::white << "Would you want to "
        << termcolor::bold << "[" << termcolor::yellow << "L" << termcolor::white << "]oad"
        << termcolor::reset << " or "
        << termcolor::bold << "[" << termcolor::yellow << "C" << termcolor::white << "]reate"
        << termcolor::reset << " a new character ?"
        << "\n";

    session_.send(os.str());
  }

  void player::process_input(std::string const& input)
  {
    switch(current_state_)
    {
      case state::login:  current_state_ = process_login(input);
                          break;

      case state::new_player: current_state_ = create_player(input);
                              break;

      case state::ask_password: current_state_ = ask_password(input);
                              break;

      case state::load_player: current_state_ = load_player(input);
                              break;

      case state::check_password: current_state_ = check_password(input);
                                  break;

      case state::play:   current_state_ = play(input);
                          break;

      default:  break;
    }
  }

  void player::send( std::string const& msg ) { session_.send(msg); }

  void player::tick() { if(current_state_ == state::play) send(info("**TICK**")); }

  state player::create_player(std::string const& input)
  {
    if(input.empty()) return state::new_player;

    name_ = input;
    send("Welcome " + input + " !\n");
    send("Choose a password: \n");
    return state::ask_password;
  }

  state player::ask_password(std::string const& input)
  {
    if(input.empty()) return state::ask_password;

    password_ = input;
    send("Your password is: " + input + "\n");
    save_player();

    send( game_context_.strings()["new_player"] );

    return state::play;
  }

  state player::process_login(std::string const& input)
  {
    game_context_.log(std::cout,"GAME") << "Login" << std::endl;

    // Input contains (L or l) / (C or c)
    if(input == "L" || input == "l")
    {
      game_context_.log(std::cout,"GAME") << "Loading character" << std::endl;
      send("Character name: ");
      return state::load_player;
    }
    else if(input == "C" || input == "c")
    {
      game_context_.log(std::cout,"GAME") << "Creating character" << std::endl;
      send("Character name: ");
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
      game_context_.log(std::cout,"PLAYER") << name_ << " quitting." << std::endl;
      send("Bye " + name_ + " !\n");
      session_.disconnect();

      return state::disconnected;
    }
    else if(input == "/shutdown")
    {
      send("Bye " + name_ + " !\nThe server will now shutdown ...\n");
      game_context_.shutdown();

      return state::disconnected;
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

  state player::load_player(std::string const& input)
  {
    auto path = game_context_.paths()["saves"] + input + ".player";
    name_     = input;

    if( game_context_.exists(*this) )
    {
      send(input + " is already logged in.\n");
      login_prompt();
      return state::login;
    }

    if( std::filesystem::exists(path) )
    {
      game_context_.script_engine().script_file(path.c_str());

      sol::table player_info = game_context_.script_engine()["player"];

      password_ = player_info[ "password" ];
      send( "Password: ");

      return state::check_password;
    }
    else
    {
      send( "Unknown player " + input + "\n"
            "Character name:"
          );

      return state::load_player;
    }
  }

  state player::check_password(std::string const& input)
  {
    if(password_ != input)
    {
      send( "Incorrect password.\n");
      send( "Password: ");

      return state::check_password;
    }
    else
    {
      send( game_context_.strings()["returning_player"] );
      return state::play;
    }
  }

  void player::save_player()
  {
    auto path = game_context_.paths()["saves"]  + name_ + ".player";
    game_context_.log(std::cout,"PLAYER") << "Saving character to: "  << path << std::endl;
    std::ofstream file( path.c_str() );

    file << "--\n";
    file << "-- MUDPP SAVE FILE\n";
    file << "--\n";
    file << "player =  { name      = \"" << name_     << "\",\n";
    file << "            password  = \"" << password_ << "\",\n";
    file << "          }\n";
  }
}
