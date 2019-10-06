#include "cxxargs.hpp"
#include <iostream>

#include "cxxargs_defined.hpp"

int main(int argc, char** argv) {
  cxxargs::cxxargs args;
  args()->parse(argc, argv);
  double val = args()->value<double>("double");
  float valf = args()->value<float>("float");
  bool valb = args()->value<bool>("gzip");
  std::vector<double> valvec = args()->value<std::vector<double>>("list");
  for (size_t i = 0; i < valvec.size(); ++i) {
    std::cout << "val nr." << i << "in list: " << valvec.at(i) << std::endl;
  }
  std::cout << val << " at pos: " << args()->get_pos("double") << std::endl;
  std::cout << valf << " at pos: " << args()->get_pos("float") << std::endl;
  std::cout << valb << std::endl;
  std::cout << args()->help() << std::endl;
  return 0;
}
