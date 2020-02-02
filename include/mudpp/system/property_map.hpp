//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#ifndef MUDPP_SYSTEM_PROPERTY_MAP_HPP_INCLUDED
#define MUDPP_SYSTEM_PROPERTY_MAP_HPP_INCLUDED

#include <sol/sol.hpp>
#include <unordered_map>
#include <string>

namespace mudpp
{
  struct property_map
  {
    std::unordered_map<std::string, sol::object> entries;

    auto begin()        { return entries.begin(); }
    auto begin()  const { return entries.begin(); }

    auto end()          { return entries.begin(); }
    auto end()    const { return entries.begin(); }

    std::size_t length() const { return entries.size(); }

    void set(std::string const& key, sol::stack_object value)
    {
      auto it = entries.find(key);
      if (it == entries.cend())
      {
        entries.insert(it, { std::move(key), std::move(value) });
      }
      else
      {
        std::pair<const std::string, sol::object>& kvp = *it;
        sol::object& entry = kvp.second;
        entry = sol::object(std::move(value));
      }
    }

    sol::object get(std::string const& key) const
    {
      auto it = entries.find(key);
      if (it == entries.cend()) return sol::lua_nil;
      return it->second;
    }
  };
}

#endif
