#include "Log.hpp"

using namespace std;

string _get_time_string() {
  time_t timestamp = time(nullptr);
  tm local_time = *localtime(&timestamp);
  stringstream s;
  s << put_time(&local_time, "%y-%m-%d %H:%M:%S");
  return s.str();
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
  info("Starting log session...");
}

void Log::set_level(const Level& level) {
  m_log_level = level;
}

string Log::build_prefix(const string &name, const Level& level) {
  string prefix;
  string level_name = get_level_name(level);

  prefix = _get_time_string();
  prefix += " " + level_name;
  prefix += " [" + name + "] - ";

  return prefix;
}

void Log::write_log(const string& name, const string& message, const Level& level, const string& color) {
  if (m_log_level != Level::DISABLED && level >= m_log_level) {
    const string prefix = build_prefix(name, level);

    cout << COLORIZE(color, prefix) << message << endl;
  }
}

void Log::critical(const string& message, const string& name) {
  write_log(name, message, Level::CRITICAL, RED_COLOR);
}

void Log::error(const string& message, const string& name){
  write_log(name, message, Level::ERROR, RED_COLOR);
}

void Log::warning(const string& message, const string& name) {
  write_log(name, message, Level::WARNING, YELLOW_COLOR);
}

void Log::info(const string& message, const string& name) {
  write_log(name, message, Level::INFO, BLUE_COLOR);
}

void Log::debug(const string& message, const string& name) {
  write_log(name, message, Level::DEBUG, WHITE_COLOR);
}
