/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef CXXARGS_CXXARGS_HPP
#define CXXARGS_CXXARGS_HPP

#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <sstream>
#include <vector>
#include <cmath>
#include <utility>

#include "exceptions.hpp"

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
  class Argument {
  public:
    virtual uint16_t get_pos() const =0;
    virtual std::string get_help() const =0;
    virtual void parse_arg(char** begin, char** end) =0;
    virtual bool is_initialized() const =0;
    virtual std::string get_name() const =0;
    template <class T> const T& get_val() const;
    template <class T, class U> void set_val(U& in_val);
  };

  template <typename T>
  class ArgumentVal : public Argument {
  private:
    std::string short_name;
    std::string long_name;
    std::string help_text;
    T val;
    uint16_t pos;
    bool value_initialized = false;

    std::pair<char**, uint16_t> FindArg(char **begin, char **end) const {
      char **it = std::find(begin, end, this->short_name);
      if (it == end) {
	it = std::find(begin, end, this->long_name);
      }
      uint16_t pos = std::ceil((std::distance(begin, it) + 1)/2.0);
      return std::make_pair(it, pos);
    }

  public:
    ArgumentVal();
    ArgumentVal(std::string short_name, std::string long_name, std::string help_text)
      : short_name("-" + short_name)
      , long_name("--" + long_name)
      , help_text(this->short_name + " " + this->long_name + "\t" + help_text) {
    }
    ArgumentVal(std::string short_name, std::string long_name, std::string help_text, T in_val)
      : ArgumentVal(short_name, long_name, help_text) {
      this->set_val(in_val);
    }
    void parse_arg(char** begin, char **end) override {
      std::pair<char**, uint16_t> at = FindArg(begin, end);
      char** it = at.first;
      if (it != end && ++it != end) {
	std::stringstream arg(*it);
	T in_val;
	arg >> in_val;
	this->set_val(in_val);
	this->pos = at.second;
      }
    }
    std::string get_help() const override { return this->help_text; }
    const T& get_val() const { return this->val; };
    void set_val(T& in_val) { this->value_initialized = true; this->val = in_val; }
    uint16_t get_pos() const override { return this->pos; }
    bool is_initialized() const override { return this->value_initialized; }
    std::string get_name() const override { return this->long_name; }
  };

  template<> void ArgumentVal<bool>::parse_arg(char** begin, char** end) {
    std::pair<char**, uint16_t> at = FindArg(begin, end);
    this->pos = at.second;
    bool in_val = ((at.first != end) ^ this->val);
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
    std::string help_text;
    std::string program_name;

  public:
    Arguments(std::string program_name, std::string usage_info)
      : program_name(program_name), help_text(usage_info) {};
    template <typename T> void add_argument(std::string short_name, std::string long_name, std::string help_text) {
      this->args.insert(std::make_pair(long_name, std::shared_ptr<Argument>(new ArgumentVal<T>(short_name, long_name, help_text))));
      this->help_text += '\n' + args.at(long_name)->get_help();
    }
    template <typename T> void add_argument(std::string short_name, std::string long_name, std::string help_text, T in_val) {
      this->add_argument<T>(short_name, long_name, help_text);
      this->args.at(long_name)->set_val<T>(in_val);
    }
    void parse(int argc, char** argv) const {
      for (auto kv : args) {
	kv.second->parse_arg(argv, argv+argc);
	#ifdef CXXARGS_EXCEPTIONS_HPP
	if (!kv.second->is_initialized()) {
	  throw exceptions::no_default_value(kv.second->get_name());
	}
	#endif
      }
    }
    std::string help() const { return this->help_text; };
    template <typename T> T value(const std::string &name) const {
      #ifdef CXXARGS_EXCEPTIONS_HPP
      if (args.find(name) == args.end()) {
	throw exceptions::argument_not_found(name);
      } else if (!args.at(name)->is_initialized()) {
	throw exceptions::value_uninitialized(name);
      }
      #endif
      return args.at(name)->get_val<T>();
    }
    uint16_t get_pos(const std::string &name) const {
      #ifdef CXXARGS_EXCEPTIONS_HPP
      if (args.find(name) == args.end()) {
	throw exceptions::value_uninitialized(name);
      }
      #endif
      return args.at(name)->get_pos();
    }
    std::string get_name() const { return this->program_name; }
  };
}

#endif
