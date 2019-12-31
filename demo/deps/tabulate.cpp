//==================================================================================================
/**
  MUPP - MUD in C++
  Copyright 2019 Joel FALCOU

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
**/
//==================================================================================================

#include <tabulate/table.hpp>

int main()
{
  tabulate::Table chart;
  chart.format()
      .font_color(tabulate::Color::white)
      .padding_left(0)
      .padding_right(0)
      .column_separator("")
      .hide_border();

  for (size_t i = 0; i < 9; ++i) {
    std::vector<std::variant<std::string, tabulate::Table>> row;
    row.push_back(std::to_string(90 - i * 10));
    for (size_t j = 0; j <= 50; ++j) {
      row.push_back(" ");
    }
    chart.add_row(row);
  }

  std::vector<std::variant<std::string, tabulate::Table>> row;
  for (int i = 0; i <= 12; ++i) {
    if ((i + 1) % 4 == 0) {
      row.push_back(std::to_string(i + 1));
    } else {
      row.push_back(" ");
    }
  }
  chart.add_row(row);
  chart.add_row({});

  chart.column(0).format().padding_left(1).padding_right(1).border_left(" ");

  for (size_t i = 1; i <= 18; ++i) {
    chart.column(i).format().width(2);
  }

  chart.column(2).format().border_color(tabulate::Color::white).border_left("|").border_top("-");
  chart.column(2)[8].format().background_color(tabulate::Color::red);
  chart.column(2)[7].format().background_color(tabulate::Color::red);

  chart.column(3)[8].format().background_color(tabulate::Color::yellow);
  chart.column(3)[7].format().background_color(tabulate::Color::yellow);
  chart.column(3)[6].format().background_color(tabulate::Color::yellow);

  chart.column(6)[8].format().background_color(tabulate::Color::red);
  chart.column(6)[7].format().background_color(tabulate::Color::red);
  chart.column(6)[6].format().background_color(tabulate::Color::red);
  chart.column(6)[5].format().background_color(tabulate::Color::red);

  chart.column(7)[8].format().background_color(tabulate::Color::yellow);
  chart.column(7)[7].format().background_color(tabulate::Color::yellow);
  chart.column(7)[6].format().background_color(tabulate::Color::yellow);
  chart.column(7)[5].format().background_color(tabulate::Color::yellow);
  chart.column(7)[4].format().background_color(tabulate::Color::yellow);

  chart.column(10)[8].format().background_color(tabulate::Color::red);
  chart.column(10)[7].format().background_color(tabulate::Color::red);
  chart.column(10)[6].format().background_color(tabulate::Color::red);
  chart.column(10)[5].format().background_color(tabulate::Color::red);
  chart.column(10)[4].format().background_color(tabulate::Color::red);
  chart.column(10)[3].format().background_color(tabulate::Color::red);

  chart.column(11)[8].format().background_color(tabulate::Color::yellow);
  chart.column(11)[7].format().background_color(tabulate::Color::yellow);
  chart.column(11)[6].format().background_color(tabulate::Color::yellow);
  chart.column(11)[5].format().background_color(tabulate::Color::yellow);
  chart.column(11)[4].format().background_color(tabulate::Color::yellow);
  chart.column(11)[3].format().background_color(tabulate::Color::yellow);
  chart.column(11)[2].format().background_color(tabulate::Color::yellow);
  chart.column(11)[1].format().background_color(tabulate::Color::yellow);

  chart[2][15].format().background_color(tabulate::Color::red);
  chart[2][16].set_text("Batch 1");
  chart.column(16).format().padding_left(1).width(20);

  chart[4][15].format().background_color(tabulate::Color::yellow);
  chart[4][16].set_text("Batch 2");

  tabulate::Table legend;
  legend.add_row({"Batch 1", "10", "40", "50", "20", "10", "50"});
  legend.add_row({"Batch 2", "30", "60", "(70)", "50", "40", "30"});

  legend[0].format().font_align(tabulate::FontAlign::center);
  legend[1].format().font_align(tabulate::FontAlign::center);

  legend.column(0)
      .format()
      .font_align(tabulate::FontAlign::right)
      .font_color(tabulate::Color::green)
      .font_background_color(tabulate::Color::grey);

  legend.column(2).format().font_color(tabulate::Color::white).font_background_color(tabulate::Color::red);

  legend[1][3].format().font_style({tabulate::FontStyle::italic}).font_color(tabulate::Color::yellow);

  chart.column(17).format().width(50);

  std::cout << chart;
  std::cout << legend << "\n\n";
}
