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
  typedef std::vector<std::string>::iterator aiter;
  class Argument {
  private:
    std::string short_name;
    std::string long_name;
    std::string help_text;

  public:
    Argument();
    Argument(std::string short_name, std::string long_name, std::string help_text)
    : short_name("-" + short_name)
    , long_name("--" + long_name)
    , help_text(this->short_name + " " + this->long_name + "\t" + help_text) {};

    virtual void FindArg(const aiter &begin, const aiter &end) =0;
    virtual const uint16_t& get_pos() const =0;
    virtual bool is_initialized() const =0;
    template <class T> const T& get_val() const;
    template <class T, class U> void set_val(U& in_val);

    std::string get_short_name() const { return this->short_name; }
    std::string get_long_name() const { return this->long_name; }
    std::string get_help() const { return this->help_text; }
  };

  template <typename T>
  class ArgumentVal : public Argument {
  private:
    std::pair<T, uint16_t> val;
    bool value_initialized = false;

  public:
    using Argument::Argument;
    ArgumentVal(std::string short_name, std::string long_name, std::string help_text, T in_val)
      : Argument(short_name, long_name, help_text) {
      this->set_val(in_val);
    }
    const T& get_val() const { return this->val.first; };
    void set_val(T& in_val, uint16_t pos = 0) { this->value_initialized = true; this->val = std::make_pair(in_val, pos); }
    const uint16_t& get_pos() const override { return this->val.second; }
    bool is_initialized() const override { return this->value_initialized; }

    void FindArg(const aiter &begin, const aiter &end) override {
      aiter it = std::find(begin, end, this->get_short_name());
      it = (it == end ? std::find(begin, end, this->get_long_name()) : it);
      T in_val;
      if (it == end) {
	for (aiter it = begin; it < end; ++it) {
	  if (it->find(this->get_long_name()) != std::string::npos) {
	    std::stringstream arg(*it);
	    std::string dump;
	    getline(arg, dump, '=');
	    arg >> in_val;
	    this->set_val(in_val, std::ceil((it - begin)/2.0));
	  }
	}
      } else if (++it != end) {
	std::stringstream arg(*it);
	arg >> in_val;
	this->set_val(in_val, std::ceil((it - begin)/2.0));
      }
    }
  };

  template<> void ArgumentVal<bool>::FindArg(const aiter &begin, const aiter &end) {
    aiter it = std::find(begin, end, this->get_long_name());
    it = (it == end ? std::find(begin, end, this->get_short_name()) : it);
    bool in_val = ((it != end) ^ this->val.first);
    this->set_val(in_val, 0);
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
      std::vector<std::string> vec(argv, argv+argc);
      for (auto kv : args) {
	kv.second->FindArg(vec.begin(), vec.end());
	#ifdef CXXARGS_EXCEPTIONS_HPP
	if (!kv.second->is_initialized()) {
	  throw exceptions::value_uninitialized(kv.second->get_long_name());
	}
	#endif
      }
    }

    std::string help() const { return this->help_text; };
    template <typename T> const T& value(const std::string &name) const {
      #ifdef CXXARGS_EXCEPTIONS_HPP
      if (args.find(name) == args.end()) {
	throw exceptions::argument_not_found(name);
      }
      #endif
      return args.at(name)->get_val<T>();
    }
    const uint16_t& get_pos(const std::string &name) const {
      #ifdef CXXARGS_EXCEPTIONS_HPP
      if (args.find(name) == args.end()) {
	throw exceptions::argument_not_found(name);
      }
      #endif
      return args.at(name)->get_pos();
    }
    std::string get_program_name() const { return this->program_name; }
  };
}

#endif
