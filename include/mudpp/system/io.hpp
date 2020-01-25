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

  inline std::string colorize( std::string const& s )
  {
    using namespace std::literals;

    std::ostringstream output_os;
    output_os << termcolor::colorize;

    enum class mode { character, foreground, background, command };

    mode m = mode::character;
    auto curr = s.begin();
    auto stop = s.end();

    while( curr != stop )
    {
      char c = *curr;

      // Check command
           if( c == '@') m = mode::foreground;
      else if( c == '$') m = mode::background;
      else if( c == '#') m = mode::command;
      else               m = mode::character;

      if(m == mode::character)
      {
        output_os << c;
        curr++;
      }
      else
      {
        curr++;
        char opt = *curr;

        if(m == mode::foreground)
        {
          switch(opt)
          {
            case 'r' : output_os << termcolor::red;     break;
            case 'g' : output_os << termcolor::green;   break;
            case 'b' : output_os << termcolor::blue;    break;
            case 'y' : output_os << termcolor::yellow;  break;
            case 'c' : output_os << termcolor::cyan;    break;
            case 'm' : output_os << termcolor::magenta; break;
            default: break;
          }
        }
        else if(m == mode::background)
        {
          switch(opt)
          {
            case 'r' : output_os << termcolor::on_red;     break;
            case 'g' : output_os << termcolor::on_green;   break;
            case 'b' : output_os << termcolor::on_blue;    break;
            case 'y' : output_os << termcolor::on_yellow;  break;
            case 'c' : output_os << termcolor::on_cyan;    break;
            case 'm' : output_os << termcolor::on_magenta; break;
            default: break;
          }
        }
        else
        {
          switch(opt)
          {
            case 'b' : output_os << termcolor::bold;    break;
            case 'i' : output_os << termcolor::italic;  break;
            case '#' : output_os << termcolor::reset;   break;
            default: break;
          }
        }

        curr++;
      }
    }

    return output_os.str();
  }

  inline std::string display_command( std::string const& s )
  {
    std::string out;
    for(auto c : s)
    {
      if( std::isupper(c) )
      {
        out += "#b[@y";
        out += c;
        out += "###b]";
      }
      else
      {
        out += c;
      }

    }

    return out;
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
