#ifndef CXXARGS_CXXARGS_HPP
#define CXXARGS_CXXARGS_HPP

#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <sstream>
#include <exception>
#include <vector>
#include <cstring>
#include <cmath>
#include <utility>

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
    template <class T> const T& get_val() const;
  };

  template <typename T>
  class ArgumentVal : public Argument {
  private:
    std::string short_name;
    std::string long_name;
    std::string help_text;
    T val;
    uint16_t pos;

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
    ArgumentVal(std::string a, std::string b, std::string c) {
      short_name = "-" + a;
      long_name = "--" + b;
      help_text = short_name + " " + long_name + "\t" + c;
    }
    ArgumentVal(std::string a, std::string b, std::string c, T in_val) : ArgumentVal(a, b, c) {
      val = in_val;
    }
    void parse_arg(char** begin, char **end) override {
      std::pair<char**, uint16_t> at = FindArg(begin, end);
      char** it = at.first;
      if (it != end && ++it != end) {
	std::stringstream arg(*it);
	arg >> this->val;
      }
      this->pos = at.second;
    }
    std::string get_help() const override { return this->help_text; }
    const T& get_val() const { return this->val; };
    uint16_t get_pos() const override { return this->pos; }
  };

  template<> void ArgumentVal<bool>::parse_arg(char** begin, char** end) {
    std::pair<char**, uint16_t> at = FindArg(begin, end);
    this->pos = at.second;
    this->val = ((at.first != end) ^ this->val);
  }

  template<class T> const T& Argument::get_val() const {
    return dynamic_cast<const ArgumentVal<T>&>(*this).get_val();
  }

  class Arguments {
  private:
    std::map<std::string, std::shared_ptr<Argument>> args;

  public:
    template <typename T> void add_argument(std::string short_name, std::string long_name, std::string help_text) {
      this->args.insert(std::make_pair(long_name, std::shared_ptr<Argument>(new ArgumentVal<T>(short_name, long_name, help_text))));
    }
    template <typename T> void add_argument(std::string short_name, std::string long_name, std::string help_text, T in_val) {
      this->args.insert(std::make_pair(long_name, std::shared_ptr<Argument>(new ArgumentVal<T>(short_name, long_name, help_text, in_val))));
    }
    void parse(int argc, char** argv) const {
      for (auto kv : args) {
	kv.second->parse_arg(argv, argv+argc);
      }
    }
    std::string help() const {
      std::string help_text("");
      for (auto kv : args) {
	help_text += kv.second->get_help();
	help_text += '\n';
      }
      return help_text;
    }
    template <typename T> T value(const std::string &name) const { return args.at(name)->get_val<T>(); }
    uint16_t get_pos(const std::string &name) const { return args.at(name)->get_pos(); }
  };
}

#endif
