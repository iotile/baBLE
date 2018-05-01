#ifndef BABLE_LINUX_ASCIIFORMAT_HPP
#define BABLE_LINUX_ASCIIFORMAT_HPP

#include <array>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include "../AbstractFormat.hpp"
#include "AsciiFormatBuilder.hpp"
#include "AsciiFormatExtractor.hpp"
#include "../../utils/stream_formats.hpp"

class AsciiFormat : public AbstractFormat {

public:
  static std::string format_bd_address(const std::array<uint8_t, 6>& bd_address_array) {
    std::stringstream bd_address;

    for(auto it = bd_address_array.rbegin(); it != bd_address_array.rend(); ++it) {
      bd_address << RAW_HEX(*it, 2);
      if (std::next(it) != bd_address_array.rend()) {
        bd_address << ":";
      }
    }

    return bd_address.str();
  }

  static std::string format_uuid(const std::vector<uint8_t>& uuid_vector) {
    std::stringstream uuid;

    for(auto& v : uuid_vector) {
      uuid << RAW_HEX(v, 2);
    }

    return uuid.str();
  }

  static std::string format_device_name(const std::vector<uint8_t>& device_name_array) {
    std::stringstream device_name;

    for(auto& v : device_name_array) {
      device_name << char(v);
    }

    return device_name.str();
  }

  const Packet::Type packet_type() const override {
    return Packet::Type::ASCII;
  };

  const size_t header_length() const override {
    return 0;
  };

  bool is_command(uint16_t type_code) override {
    return true;
  };

  bool is_event(uint16_t type_code) override {
    return false;
  };

  uint16_t extract_command_code(const std::vector<uint8_t>& data) override {
    return AsciiFormatExtractor::extract_command_code(data);
  };

  uint16_t extract_type_code(const std::vector<uint8_t>& data) override {
    return 0;
  };

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    throw std::runtime_error("Ascii format can't extract payload length: this information is not included in the format.");
  };

};

#endif //BABLE_LINUX_ASCIIFORMAT_HPP
