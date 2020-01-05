//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright 2019-2020 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#include <tabulate/termcolor.hpp>
#include <tabulate/table.hpp>
#include <sstream>

namespace mudpp
{
  template<typename T> inline std::string urgent(T const& data)
  {
    std::ostringstream stream;
    stream  << termcolor::colorize << termcolor::red << termcolor::bold
            << data << termcolor::reset << '\n';

    return stream.str();
  }

  template<typename T> inline std::string info(T const& data)
  {
    std::ostringstream stream;
    stream  << termcolor::colorize << termcolor::yellow << termcolor::bold
            << data << termcolor::reset << '\n';

    return stream.str();
  }

  template<typename... Contents>
  inline std::string box_message( std::tuple<std::string,tabulate::Color> const& corners
                                , std::tuple<std::string,tabulate::Color> const& borders
                                , std::tuple< std::vector<tabulate::FontStyle>
                                            , tabulate::Color
                                            , tabulate::FontAlign
                                            > const& styles
                                , int width
                                , Contents const&... contents
                                )
  {
    tabulate::Table msg;

    msg .format()
        .multi_byte_characters(true)
        .width(width)
        .font_style(std::get<0>(styles))
        .font_color(std::get<1>(styles))
        .font_align(std::get<2>(styles))
        .corner(std::get<0>(corners)).corner_color(std::get<1>(corners))
        .border(std::get<0>(borders)).border_color(std::get<1>(borders));

    int k = 1;
    msg.add_row({" "});

    ((msg.add_row({contents}), msg[k++].format().hide_border_top()), ...);

    msg.add_row({" "});
    msg[k].format().hide_border_top();

    std::ostringstream os;
    os << termcolor::colorize << msg << "\n";

    return os.str();
  }
}
