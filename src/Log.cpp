#include "Log.h"

using namespace std;

string _get_time_string() {
  std::time_t timestamp = time(nullptr);
  std::string s = ctime(&timestamp);
  return s.substr(0, s.size() - 1);
}

string Log::get_level_name(const Level& level) {
  switch (level) {
    case Level::CRITICAL:
      return "CRI";
    case Level::ERROR:
      return "ERR";
    case Level::WARNING:
      return "WAR";
    case Level::INFO:
      return "INF";
    case Level::DEBUG:
      return "DEB";
    default:
      return " - ";
  }
}

Log::Log() {
  set_level(Level::INFO);
  set_prefix("General", true);
  log_info("Starting log session...");
}

void Log::set_level(const Level& level) {
  m_log_level = level;
}

void Log::set_prefix(const string& name, bool include_timestamp) {
  m_name = name;
  m_include_timestamp = include_timestamp;
}

string Log::get_prefix(const Level& level) {
  string prefix;
  string level_name = get_level_name(level);

  if (m_include_timestamp) {
    prefix += _get_time_string() + " ";
  }

  prefix += level_name + " [" + m_name + "] - ";

  return prefix;
}

void Log::write_log(const string& message, const Level& level, const string& color) {
  if (m_log_level != Level::DISABLED && level >= m_log_level) {
    const string prefix = get_prefix(level);

    cout << COLORIZE(color, prefix) << message << endl;
  }
}

void Log::log_critical(const string& message) {
  write_log(message, Level::CRITICAL, RED_COLOR);
}

void Log::log_error(const string& message) {
  write_log(message, Level::ERROR, RED_COLOR);
}

void Log::log_warning(const string& message) {
  write_log(message, Level::WARNING, YELLOW_COLOR);
}

void Log::log_info(const string& message) {
  write_log(message, Level::INFO, BLUE_COLOR);
}

void Log::log_debug(const string& message) {
  write_log(message, Level::DEBUG, WHITE_COLOR);
}

