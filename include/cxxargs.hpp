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

namespace std {
  template <typename T> istream& operator>> (istream &in, vector<T> &t) {
    T in_val;
    string str;;
    while (getline(in, str, ',')) {
      stringstream sstream(str);
      sstream >> in_val;
      t.emplace_back(in_val);
    }
    return in;
  }
}
namespace cxxargs {
  class Argument {
  public:
    virtual std::string get_help() =0;
    virtual void parse_arg(char** begin, char** end) =0;
    virtual operator bool() { throw std::exception(); };
    virtual operator float() { throw std::exception(); };
    virtual operator double() { throw std::exception(); };
    virtual operator std::vector<double>() {throw std::exception();};
  };

  template <typename T>
  class ArgumentVal : public Argument {
  private:
    std::string short_name;
    std::string long_name;
    std::string help_text;
    T val;

    char** FindArg(char **begin, char **end) {
      char **it = std::find(begin, end, this->short_name);
      if (it == end) {
	it = std::find(begin, end, this->long_name);
      }
      return it;
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
      char** it = FindArg(begin, end);
      if (it != end && ++it != end) {
	std::stringstream arg(*it);
	arg >> this->val;
      }
    }
    std::string get_help() override { return this->help_text; }
    operator T() override { return this->val; }
  };
  template<> void ArgumentVal<bool>::parse_arg(char** begin, char** end) {
    char **it = FindArg(begin, end);
    this->val = ((it != end) ^ this->val);
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
    void parse(int argc, char** argv) {
      for (auto kv : args) {
	kv.second->parse_arg(argv, argv+argc);
      }
    }
    template <typename T> T value(const std::string &name) { return (*args.at(name)); }
    std::string help() {
      std::string help_text("");
      for (auto kv : args) {
	help_text += kv.second->get_help();
	help_text += '\n';
      }
      return help_text;
    }
  };
}

#endif
