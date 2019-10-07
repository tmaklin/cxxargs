#include "cxxargs.hpp"
#include <iostream>

#include "cxxargs_defined.hpp"

int main(int argc, char** argv) {
  cxxargs::Arguments args;
  args.add_argument<double>("d", "double", "This is a double.");
  args.add_argument<bool>("gz", "gzip", "This is a boolean toggle.");
  args.add_argument<std::string>("s", "string", "This is a string.");
  args.add_argument<std::vector<int>>("l", "list", "This is a list of integers.");
  args.parse(argc, argv);
  double val = args.value<double>("double");
  std::cout << "Value of the double: " << val << std::endl;
  bool gzip = args.value<bool>("gzip");
  std:: cout << "Toggle is: " << (gzip ? "true" : "false") << std::endl;
  std::string str = args.value<std::string>("string");
  std:: cout << "String reads: " << str << std::endl;
  std::vector<int> ints = args.value<std::vector<int>>("list");
  for (size_t i = 0; i < ints.size(); ++i) {
    std::cout << "Integer in the list at pos " << i << " is: " << ints.at(i) << std::endl;
  }

  return 0;
}
