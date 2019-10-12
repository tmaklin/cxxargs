/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <iostream>

#include "cxxargs.hpp"

int main(int argc, char** argv) {
  cxxargs::Arguments args("cxxargs command line parser", "Usage: ./test --string abc");
  std::cout << args.get_program_name() << std::endl;
  args.add_argument<double>("d", "double", "This is a double.", 0.222);
  args.add_argument<bool>("g", "gzip", "This is a boolean toggle.");
  args.add_argument<std::string>("s", "string", "This is a string.");
  args.add_argument<std::vector<int>>("l", "list", "This is a list of integers.");
  args.add_argument<bool>("t", "toggle", "This is another toggle.");
  args.add_argument<uint16_t>("a", "argument", "This is a uint16_t argument.");
  args.parse(argc, argv);

  double val = args.value<double>("double");
  std::cout << "Value of the double: " << val << std::endl;

  bool gzip = args.value<bool>("gzip");
  std::cout << "Toggle --gzip: " << (gzip ? "true" : "false") << std::endl;

  std::string str = args.value<std::string>("string");
  std::cout << "Text in the string: " << str << std::endl;

  std::vector<int> ints = args.value<std::vector<int>>("list");
  std::cout << "List of integers:" << std::endl;
  for (size_t i = 0; i < ints.size(); ++i) {
    std::cout << '\t' << ints.at(i) << " at position " << i << std::endl;
  }

  bool toggle = args.value<bool>("toggle");
  std::cout << "Toggle -t: " << (toggle ? "true" : "false") << std::endl;

  uint16_t argument = args.value<uint16_t>("argument");
  std::cout << "Argument -a: " << argument << std::endl;

  if (args.get_n_positionals() > 0) {
    for (size_t i = 0; i < args.get_n_positionals(); ++i) {
      std::cout << "Positional argument " << i << " " << args.get_positional(i) << std::endl;
    }
  }
  std::cout << args.help() << std::endl;

  return 0;
}
