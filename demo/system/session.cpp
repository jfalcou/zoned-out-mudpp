//==================================================================================================
/**
  MUPP - MUD in C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <mudpp/system/session_manager.hpp>
#include <boost/asio.hpp>
#include <iostream>

int main(int, char**)
{
  boost::asio::io_service io_service;

  try
  {
    mudpp::session_manager ts(io_service, 4000 );
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "TELNET SERVER ERROR: " << e.what() << std::endl;
  }

  return 0;
}
