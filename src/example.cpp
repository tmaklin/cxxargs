/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <iostream>

#include "cxxargs.hpp"

int main(int argc, char** argv) {
  cxxargs::Arguments args;
  args.add_argument<double>("d", "double", "This is a double.");
  args.add_argument<bool>("gz", "gzip", "This is a boolean toggle.");
  args.add_argument<std::string>("s", "string", "This is a string.");
  args.add_argument<std::vector<int>>("l", "list", "This is a list of integers.");
  args.parse(argc, argv);

  double val = args.value<double>("double");
  std::cout << "Value of the double (argument nr. " << args.get_pos("double") << "): " << val << std::endl;

  bool gzip = args.value<bool>("gzip");
  std::cout << "Toggle --gzip (argument nr. " << args.get_pos("gzip") << "): " << (gzip ? "true" : "false") << std::endl;

  std::string str = args.value<std::string>("string");
  std::cout << "Text in the string (argument nr. " << args.get_pos("string") << "): " << str << std::endl;

  std::vector<int> ints = args.value<std::vector<int>>("list");
  std::cout << "List of integers (argument nr. " << args.get_pos("list") << "):" << std::endl;
  for (size_t i = 0; i < ints.size(); ++i) {
    std::cout << '\t' << ints.at(i) << " at position " << i << std::endl;
  }

  return 0;
}
