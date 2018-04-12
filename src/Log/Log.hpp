#ifndef BABLE_LINUX_LOG_HPP
#define BABLE_LINUX_LOG_HPP

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "Loggable.hpp"
#include "../utils/colors.hpp"

#define DEFAULT_NAME "General"

class Log {

public:
  inline static Log& get_instance() {
    static Log instance;
    return instance;
  }

  enum Level: unsigned char {
    CRITICAL = 50,
    ERROR = 40,
    WARNING = 30,
    INFO = 20,
    DEBUG = 10,
    DISABLED = 0
  };

  void set_level(const Level& level);

  void write_log(const std::string &name, const std::string& message, const Level& level, const std::string& color = WHITE_COLOR);
  void critical(const std::string& message, const std::string &name = DEFAULT_NAME);
  void error(const std::string& message, const std::string &name = DEFAULT_NAME);
  void warning(const std::string& message, const std::string &name = DEFAULT_NAME);
  void info(const std::string& message, const std::string &name = DEFAULT_NAME);
  void debug(const std::string& message, const std::string &name = DEFAULT_NAME);
  void debug(const Loggable& object, const std::string &name = DEFAULT_NAME);

private:
  Log();
  Log(Log const&);
  Log& operator=(Log const&);

  std::string build_prefix(const std::string &name, const Level& level);
  std::string get_level_name(const Level& level);

  unsigned char m_log_level;
};

#define LOG Log::get_instance()
#define ENABLE_LOGGING(lvl) LOG.set_level(Log::Level::lvl)
#define DISABLE_LOGGING LOG.set_level(Log::Level::DISABLED)

#endif //BABLE_LINUX_LOG_HPP
