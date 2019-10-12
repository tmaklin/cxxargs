/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef CXXARGS_CXXARGS_HPP
#define CXXARGS_CXXARGS_HPP

#include <string>
#include <algorithm>
#include <map>
#include <sstream>
#include <vector>
#include <cmath>
#include <utility>
#include <exception>

namespace std {
  template <typename T> istream& operator>> (istream &in, vector<T> &t) {
    string str;
    while (getline(in, str, ',')) {
      t.emplace_back(*(new T()));
      stringstream sstream(str);
      sstream >> t.back();
    }
    return in;
  }
}

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
	msg += "Value of --" + name + " has not been set and has no default value.";
      }
    };
  }

  class Argument {
  private:
    char short_name;
    std::string long_name;
    std::string help_text;

  public:
    Argument();
    Argument(char short_name, std::string long_name, std::string help_text)
      : short_name(short_name)
      , long_name("--" + long_name)
      , help_text(std::string(1, this->short_name) + " " + this->long_name + "\t" + help_text) {};

    virtual void parse_argument(std::vector<std::string>::const_iterator iter) =0;
    virtual void parse_argument(std::stringstream &str) =0;
    virtual const bool& is_initialized() const =0;
    template <class T> const T& get_val() const;
    template <class T, class U> void set_val(U& in_val);

    const std::string& get_help() const { return this->help_text; }
  };

  template <typename T>
  class ArgumentVal : public Argument {
  private:
    T val;
    bool value_initialized = false;

  public:
    using Argument::Argument;
    ArgumentVal(char short_name, std::string long_name, std::string help_text, T in_val)
      : Argument(short_name, long_name, help_text) {
      this->set_val(in_val);
    }
    const T& get_val() const { return this->val; };
    void set_val(T& in_val) { this->value_initialized = true; this->val = in_val; }
    const bool& is_initialized() const override { return this->value_initialized; }

    void parse_argument(std::stringstream &str) override {
      T in_val;
      str >> in_val;
      this->set_val(in_val);
    }

    void parse_argument(std::vector<std::string>::const_iterator iter) override {
      ++iter;
      std::stringstream str(*iter);
      parse_argument(str);
    }
  };

  template<> void ArgumentVal<bool>::parse_argument(std::vector<std::string>::const_iterator iter) {
    bool in_val = (this->is_initialized() ? !this->get_val() : true);
    this->set_val(in_val);
  }
  
  template<class T> const T& Argument::get_val() const {
    return dynamic_cast<const ArgumentVal<T>&>(*this).get_val();
  }
  template<class T, class U> void Argument::set_val(U& in_val) {
    return dynamic_cast<ArgumentVal<T>&>(*this).set_val(in_val);
  }

  class Arguments {
  private:
    std::map<std::string, std::shared_ptr<Argument>> args;
    std::map<char, std::shared_ptr<Argument>> shortargs;
    std::vector<std::string> positionals;
    std::string help_text;
    std::string program_name;

  public:
    Arguments(std::string program_name, std::string usage_info)
      : program_name(program_name), help_text(usage_info) {};
    template <typename T> void add_argument(char short_name, std::string long_name, std::string help_text) {
      this->args.insert(std::make_pair("--" + long_name, std::shared_ptr<Argument>(new ArgumentVal<T>(short_name, long_name, help_text))));
      this->shortargs.insert(std::make_pair(short_name, this->args.at("--" + long_name)));
      this->help_text += '\n' + args.at("--" + long_name)->get_help();
    }
    template <typename T> void add_argument(char short_name, std::string long_name, std::string help_text, T in_val) {
      this->add_argument<T>(short_name, long_name, help_text);
      this->args.at("--" + long_name)->set_val<T>(in_val);
    }
    void parse(int argc, char** argv) {
      std::vector<std::string> vec(argv, argv+argc);      
      for (std::vector<std::string>::const_iterator it = vec.begin() + 1; it < vec.end(); ++it) {
	if (this->args.find(*it) != this->args.end()) {
	  this->args.at(*it)->parse_argument(it);
	} else if (it->compare(0, 1, "-") == 0 && it->compare(1, 1, "-") != 0) {
	  for (size_t i = 1; i < it->size(); ++i) {
	    if (this->shortargs.find(it->at(i)) != this->shortargs.end()) {
	      this->shortargs.at(it->at(i))->parse_argument(it);
	    }
	  }
	} else if (it->find('=') != std::string::npos) {
	  std::stringstream arg(*it);
	  std::string name;
	  getline(arg, name, '=');
	  if (this->args.find(name) != this->args.end()) {
	    this->args.at(name)->parse_argument(arg);
	  }
	} else if (it->compare("--") == 0) {
	  while (++it < vec.end()) {
	    this->positionals.emplace_back(*it);
	  }
	}
      }
    }

    const std::string& help() const { return this->help_text; };
    template <typename T> const T& value(const std::string &name) const {
      if (this->args.find("--" + name) == this->args.end()) {
	throw exceptions::argument_not_found(name);
      } else if (!this->args.at("--" + name)->is_initialized()) {
	throw exceptions::value_uninitialized(name);
      }
      return this->args.at("--" + name)->get_val<T>();
    }
    const std::string& get_program_name() const { return this->program_name; }
    const std::string& get_positional(const uint16_t &pos) const { return this->positionals.at(pos); }
    size_t n_positionals() const { return this->positionals.size(); }
  };
}

#endif
