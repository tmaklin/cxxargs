#ifndef CXXARGS_POSSIBLE_HPP
#define CXXARGS_POSSIBLE_HPP

#include "cxxargs.hpp"

namespace cxxargs {
  class cxxargs {
  private:
    Arguments args;
  public:
    cxxargs() {
      args.add_argument<double>("d",  "double", "This is a double.");
      args.add_argument<float>("f",  "float", "This is a float.");
      args.add_argument<bool>("gzip", "gzip", "This is a bool.");
      args.add_argument<std::vector<double>>("l", "list", "This is a list of integers");
    }
    Arguments* operator()() { return &this->args; }
  };
}
#endif

