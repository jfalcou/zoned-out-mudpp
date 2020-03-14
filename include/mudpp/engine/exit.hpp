//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_ENGINE_EXIT_HPP_INCLUDED
#define MUDPP_ENGINE_EXIT_HPP_INCLUDED

#include <sol/sol.hpp>
#include <string>

namespace mudpp
{

  struct exit
  {
    enum class state { none, open, closed, unlocked, locked, direct };

    exit();
    exit(sol::table const& data);

    static void setup_scripting( sol::usertype<exit>&, sol::state& );

    void tick();

    void lock(player*);
    void unlock(player*);
    void open(player*);
    void close(player*);

    int traverse(player*) const;
    std::string describe() const noexcept;

    std::string const&        type()        const noexcept { return type_; }
    std::string const&        description() const noexcept { return description_; }

    void set_type(std::string const& n)         { type_ = n; }
    void set_description(std::string const& d)  { description_ = d; }

    private:
    std::string description_;
    std::string type_;
    state       state_;
    int         target_room_;
  };
}

#endif

