//==================================================================================================
/**
  MUPP - MUD in C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/io/session_manager.hpp>
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
  mudpp::session_manager s(4000);

  do
  {
    s.handle_connections  ();
  }while(s.connection_count() < 10);

  return 0;
}

