//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_PLAYER_HPP_INCLUDED
#define MUDPP_ENGINE_PLAYER_HPP_INCLUDED

#include <mudpp/system/session.hpp>
#include <string>
#include <memory>

namespace mudpp
{
  enum class state : std::uint8_t
  {
    login,
    load_player,
    check_password,
    new_player,
    ask_password,
    play,
    disconnected
  };

  struct game;

  struct player
  {
    player(session& s);

    static std::unique_ptr<player> make(session& s);
    void login_prompt();

    void process_input(std::string const& input);
    void tick();
    void send( std::string const& msg );

    std::string const& name() const { return name_; }

    state current_state() const { return current_state_; }
    bool is_connected() const { return current_state_ != state::disconnected; }
    bool is_logged()    const { return current_state_ == state::play; }

    private:
    state process_login(std::string const& input);
    state play(std::string const& input);
    state create_player(std::string const& input);
    state load_player(std::string const& input);
    state ask_password(std::string const& input);
    state check_password(std::string const& input);

    void save_player();

    session&    session_;
    game&       game_context_;
    state       current_state_;
    std::string name_, password_;
  };

  using player_t = std::unique_ptr<player>;
}

#endif
