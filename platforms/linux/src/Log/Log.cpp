#include "Log.hpp"

using namespace std;

string _get_time_string() {
  time_t timestamp = time(nullptr);
  tm* local_time = localtime(&timestamp);
  char result[20];
  strftime (result, sizeof(result), "%y-%m-%d %H:%M:%S", local_time);

  return string(result);
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
}

void Log::set_level(const Level& level) {
  m_log_level = level;
}

void Log::set_level(const string& str_level) {
  if (str_level == "DEBUG") {
    m_log_level = Level::DEBUG;
  } else if (str_level == "INFO") {
    m_log_level = Level::INFO;
  } else if (str_level == "WARNING") {
    m_log_level = Level::WARNING;
  } else if (str_level == "ERROR") {
    m_log_level = Level::ERROR;
  } else if (str_level == "CRITICAL") {
    m_log_level = Level::CRITICAL;
  } else if (str_level == "NOTSET") {
    m_log_level = Level::NOTSET;
  } else {
    throw invalid_argument("Wrong log level.");
  }
}

string Log::build_prefix(const string &name, const Level& level) {
  string prefix;
  string level_name = get_level_name(level);

  prefix = _get_time_string();
  prefix += " " + level_name;
  prefix += " [" + name + "]";

  return prefix;
}

void Log::write_log(const string& name, const string& message, const Level& level, const string& color) {
  if (m_log_level != Level::NOTSET && level >= m_log_level) {
    const string prefix = build_prefix(name, level);
    const string log = COLORIZE(color, prefix) + " - " + message;

    fprintf(stderr, "%s\n", log.c_str());
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

void Log::debug(const Loggable& object, const string& name) {
  debug(object.stringify(), name);
}

void Log::debug(const vector<uint8_t>& bytes, const string &name) {
  stringstream message_stream;
  message_stream << "[ ";
  for(const uint8_t& value : bytes) {
    message_stream << HEX(value) << " ";
  }
  message_stream << "]";
  debug(message_stream.str(), name);
}
