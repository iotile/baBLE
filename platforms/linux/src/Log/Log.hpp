#ifndef BABLE_LINUX_LOG_HPP
#define BABLE_LINUX_LOG_HPP

#include <vector>
#include "Loggable.hpp"
#include "../utils/colors.hpp"
#include "../utils/stream_formats.hpp"

#define DEFAULT_NAME "General"

// Singleton used to log messages with colors and using a coherent format
class Log {

public:
  inline static Log& get() {
    static Log instance;
    return instance;
  }

  enum Level: unsigned char {
    CRITICAL = 50,
    ERROR = 40,
    WARNING = 30,
    INFO = 20,
    DEBUG = 10,
    NOTSET = 0
  };

  void set_level(const Level& level);
  void set_level(const std::string& level);

  void write_log(const std::string &name, const std::string& message, const Level& level, const std::string& color = WHITE_COLOR);
  void critical(const std::string& message, const std::string &name = DEFAULT_NAME);
  void error(const std::string& message, const std::string &name = DEFAULT_NAME);
  void warning(const std::string& message, const std::string &name = DEFAULT_NAME);
  void info(const std::string& message, const std::string &name = DEFAULT_NAME);
  void debug(const std::string& message, const std::string &name = DEFAULT_NAME);
  void debug(const Loggable& object, const std::string &name = DEFAULT_NAME);
  void debug(std::vector<uint8_t> bytes, const std::string &name = DEFAULT_NAME);

  template<size_t N>
  void debug(std::array<uint8_t, N> bytes, const std::string &name = DEFAULT_NAME);

private:
  Log();
  Log(Log const&);
  Log& operator=(Log const&);

  std::string build_prefix(const std::string &name, const Level& level);
  std::string get_level_name(const Level& level);

  unsigned char m_log_level;
};

template<size_t N>
void Log::debug(std::array<uint8_t, N> bytes, const std::string &name) {
  std::stringstream message_stream;
  message_stream << "[ ";
  for(uint8_t& value : bytes) {
    message_stream << HEX(value) << " ";
  }
  message_stream << "]";
  debug(message_stream.str(), name);
};

#define LOG Log::get()

#endif //BABLE_LINUX_LOG_HPP
