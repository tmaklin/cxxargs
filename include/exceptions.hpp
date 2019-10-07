/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef CXXARGS_EXCEPTIONS_HPP
#define CXXARGS_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace cxxargs {
  namespace exceptions {
    struct cxxargs_exception : public std::exception {
      std::string msg;
      const char* what() const throw() { return msg.c_str(); };
    };

    struct argument_not_found : public cxxargs_exception {
      argument_not_found(const std::string &name) {
	msg += "Argument --" + name + " is not defined.";
      }
    };

    struct value_uninitialized : public cxxargs_exception {
      value_uninitialized(const std::string &name) {
	msg += "Value of --" + name + " has not been set.";
      }
    };
  }
}

#endif
