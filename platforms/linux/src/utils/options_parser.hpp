#ifndef BABLE_OPTIONS_PARSER_HPP
#define BABLE_OPTIONS_PARSER_HPP

#include <map>

namespace Utils {

  struct Option {
    std::string name;
    void (* func)(char* param);
    std::string doc;
    std::string param_name;
  };

  void display_version(char* param);
  void set_logging_level(char* param);
  void disable_signal(char* param);
  void set_expiration_time(char* param);

  std::map<std::string, int> parse_options(int argc, char* argv[]);

  static std::map<std::string, int> settings = {
      {"handle_sigint", true},
      {"expiration_time", 60 * 1000}
  };

  static std::vector<Option> possible_options{
      {"--version", display_version, "display version of bable-linux-bridge"},
      {"--logging", set_logging_level, "set logging level to LEVEL (values: DEBUG|INFO|WARNING|ERROR|CRITICAL|NOTSET)", "LEVEL"},
      {"--disable-signal", disable_signal, "do not handle SIGINT signal (used to cleanly stop on Ctrl-C)"},
      {"--expiration-time", set_expiration_time, "set time before packets waiting for response expire (milliseconds)", "TIME"}
  };

}

#endif //BABLE_OPTIONS_PARSER_HPP
