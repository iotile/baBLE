#include <sstream>
#include <Log/Log.hpp>
#include "options_parser.hpp"

using namespace std;

namespace Utils {

  void display_version(char* param) {
    fprintf(stdout, "%s\n", VERSION);
    exit(0);
  }

  void set_logging_level(char* param) {
    LOG.set_level(string(param));
  }

  void disable_signal(char* param) {
    settings.at("handle_sigint") = false;
  }

  void set_expiration_time(char* param) {
    try {
      settings.at("expiration_time") = stoi(string(param));
    } catch (const invalid_argument& err) {
      throw invalid_argument("Expiration time must be a valid number");
    }
  }

  void display_usage() {
    stringstream usage;
    stringstream options;

    for (const Option& option : possible_options) {
      usage << " [" << option.name;
      if (!option.param_name.empty()) {
        usage << " <" << option.param_name << ">";
      }
      usage << "]";

      options << "  " << left << setw(20) << option.name << option.doc << endl;
    }

    stringstream result;
    result << "usage: bable-bridge-linux [--help]" << usage.str() << endl
           << "options:" << endl
           << options.str() << endl;

    fprintf(stdout, "%s", result.str().c_str());
    exit(0);
  }

  map<string, int> parse_options(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {

      string option_name = string(argv[i]);
      if (option_name == "--help") {
        display_usage();
      }

      for (const Option& option : possible_options) {
        if (option.name == option_name) {
          if (option.param_name.empty()) {
            option.func(nullptr);
          } else {
            if (i + 1 < argc) {
              option.func(argv[++i]);
            } else {
              throw invalid_argument("Invalid option: " + option_name + " option requires a value");
            }
          }
          break;
        }
      }

    }

    return settings;
  }

}

