#include "cxxargs.hpp"
#include <iostream>

int main(int argc, char** argv) {
  cxxargs::Arguments args;
  args.add_option<double>(std::string("d"),  std::string("double"), std::string("This is a double."));
  args.add_option<float>(std::string("f"),  std::string("float"), std::string("This is a float."));
  args.add_option<bool>(std::string("gzip"), std::string("gzip"), std::string("This is a bool."));
  args.add_option<std::vector<double>>(std::string("l"), std::string("list"), std::string("This is a list of integers"));
  args.parse(argc, argv);
  double val = args.value<double>("double");
  float valf = args.value<float>("float");
  bool valb = args.value<bool>("gzip");
  std::vector<double> valvec = args.value<std::vector<double>>("list");
  for (size_t i = 0; i < valvec.size(); ++i) {
    std::cout << "val nr." << i << "in list: " << valvec.at(i) << std::endl;
  }
  std::cout << val << std::endl;
  std::cout << valf << std::endl;
  std::cout << valb << std::endl;
  std::cout << args.help() << std::endl;
  return 0;
}
