#ifndef BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
#define BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include "../../Packet/constants.hpp"

class AsciiFormatExtractor {

public:
  static uint16_t extract_command_code(const std::vector<uint8_t>& data) {
    if (data.empty()) {
      throw std::invalid_argument("Given ASCII data are too small (< 1 bytes). Can't extract command code.");
    }

    std::string data_str;
    for (auto& value : data) {
      auto c = static_cast<char>(value);

      if (c == Packet::Commands::Ascii::Delimiter) {
        break;
      }

      data_str += c;
    }

    return static_cast<uint16_t>(stoi(data_str));
  };

  explicit AsciiFormatExtractor(const std::vector<uint8_t>& data) {
    m_stack_pointer = 0;
    parse_payload(data);
  };

  void parse_payload(const std::vector<uint8_t>& data) {
    std::string data_str;

    for (auto& value : data) {
      auto c = static_cast<char>(value);

      if (c == Packet::Commands::Ascii::Delimiter) {
        m_payload.push_back(data_str);
        data_str.clear();
        continue;
      }

      data_str += c;
    }

    m_payload.push_back(data_str);
  }

  std::string get() {
    if (m_stack_pointer >= m_payload.size()) {
      throw std::runtime_error("No more data to get from AsciiFormatExtractor");
    }

    std::string result = m_payload.at(m_stack_pointer);
    m_stack_pointer++;

    return result;
  }


private:
  std::vector<std::string> m_payload;
  size_t m_stack_pointer;

};

#endif //BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
