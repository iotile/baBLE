#ifndef BABLE_LINUX_LOG_H
#define BABLE_LINUX_LOG_H

#include <ctime>
#include <iostream>
#include <string>
#include "utils/colors.h"

class Log {

public:
  inline static Log& get_instance() {
    static Log instance;
    return instance;
  }

  enum Level: uint8_t {
    CRITICAL = 50,
    ERROR = 40,
    WARNING = 30,
    INFO = 20,
    DEBUG = 10,
    DISABLED = 0
  };

  void set_level(const Level& level);
  void set_prefix(const std::string& name, bool include_timestamp = true);

  void write_log(const std::string& message, const Level& level, const std::string& color = WHITE_COLOR);
  void log_critical(const std::string& message);
  void log_error(const std::string& message);
  void log_warning(const std::string& message);
  void log_info(const std::string& message);
  void log_debug(const std::string& message);

private:
  Log();
  Log(Log const&);
  Log& operator=(Log const&);

  std::string get_prefix(const Level& level);
  std::string get_level_name(const Level& level);

  int8_t m_log_level;
  std::string m_name;
  bool m_include_timestamp;
};

#define LOG_INSTANCE Log::get_instance()
#define critical(msg) LOG_INSTANCE.log_critical(msg)
#define error(msg) LOG_INSTANCE.log_error(msg)
#define warn(msg) LOG_INSTANCE.log_warning(msg)
#define info(msg) LOG_INSTANCE.log_info(msg)
#define debug(msg) LOG_INSTANCE.log_debug(msg)

#define ENABLE_LOGGING(lvl) LOG_INSTANCE.set_level(Log::Level::lvl)
#define DISABLE_LOGGING LOG_INSTANCE.set_level(Log::Level::DISABLED)

#endif //BABLE_LINUX_LOG_H
