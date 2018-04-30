#ifndef BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
#define BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include "../../Packet/constants.hpp"

class AsciiFormatExtractor {

public:
  static uint16_t extract_command_code(const std::vector<uint8_t>& data) {
    if (data.size() < 2) {
      throw std::invalid_argument("Given MGMT data are too small (< 2 bytes). Can't extract command code.");
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

private:
  std::vector<std::string> m_payload;

};

#endif //BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
