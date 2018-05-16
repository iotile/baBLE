#ifndef BABLE_LINUX_HCIFORMATEXTRACTOR_HPP
#define BABLE_LINUX_HCIFORMATEXTRACTOR_HPP

#include <array>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <vector>
#include "./constants.hpp"
#include "../../Log/Log.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

class HCIFormatExtractor {

public:
  static uint8_t extract_type_code(const std::vector<uint8_t>& data) {
    if (data.empty()) {
      throw Exceptions::WrongFormatException("Given HCI data are too small (< 1 bytes). Can't extract type code.");
    }

    uint8_t type_code = data.at(0);

    return type_code;
  };

  static uint16_t extract_command_code(const std::vector<uint8_t>& data) {
    uint16_t command_code;
    uint8_t type_code = extract_type_code(data);

    switch (type_code) {
      case Format::HCI::Type::Command:
        if (data.size() < 3) {
          throw Exceptions::WrongFormatException("Given HCI data are too small (< 3 bytes). Can't extract command code from command packet.");
        }
        // Use little endian
        command_code = (data.at(2) << 8) | data.at(1);
        break;

      case Format::HCI::Type::AsyncData:
        if (data.size() < 10) {
          throw Exceptions::WrongFormatException("Given HCI data are too small (< 10 bytes). Can't extract command code from async data packet.");
        }
        command_code = static_cast<uint16_t>(data.at(10));
        break;

      case Format::HCI::Type::Event:
        if (data.size() < 2) {
          throw Exceptions::WrongFormatException("Given HCI data are too small (< 2 bytes). Can't extract command code from event packet.");
        }
        command_code = static_cast<uint16_t>(data.at(1));
        break;

      default:
        throw Exceptions::WrongFormatException("Can't extract command code from given HCI data: unknown type code.");
    }

    return command_code;
  };

  static uint16_t extract_controller_id(const std::vector<uint8_t>& data) {
    uint16_t controller_id;
    uint8_t type_code = extract_type_code(data);

    if (type_code != Format::HCI::Type::AsyncData) {
      return Format::HCI::non_controller_id;
    }

    if (data.size() < 3) {
      throw Exceptions::WrongFormatException("Given HCI data are too small (< 3 bytes). Can't extract controller id.");
    }

    // Use little endian
    controller_id = (static_cast<uint16_t>(data.at(2) & 0x0F) << 8) | data.at(1);

    return controller_id;
  };

  static uint16_t extract_payload_length(const std::vector<uint8_t>& data) {
    uint16_t payload_length;
    uint8_t type_code = extract_type_code(data);

    switch (type_code) {
      case Format::HCI::Type::Command:
        if (data.size() < 4) {
          throw Exceptions::WrongFormatException("Given HCI data are too small (< 4 bytes). Can't extract payload length from command packet.");
        }
        // Use little endian
        payload_length = static_cast<uint16_t>(data.at(3));
        break;

      case Format::HCI::Type::AsyncData:
        if (data.size() < 5) {
          throw Exceptions::WrongFormatException("Given HCI data are too small (< 5 bytes). Can't extract payload length from async data packet.");
        }
        // Use little endian
        payload_length = (data.at(4) << 8) | data.at(3);
        break;

      case Format::HCI::Type::Event:
        if (data.size() < 4) {
          throw Exceptions::WrongFormatException("Given HCI data are too small (< 4 bytes). Can't extract payload length from event packet.");
        }
        // Use little endian
        payload_length = (data.at(3) << 8) | data.at(2);
        break;

      default:
        throw Exceptions::WrongFormatException("Can't extract payload length from given HCI data: unknown type code.");
    }

    return payload_length;
  };

  // Constructors
  explicit HCIFormatExtractor(const std::vector<uint8_t>& data) {

  };

private:
  uint16_t m_event_code;
  uint16_t m_controller_id;
  uint16_t m_params_length;

  std::vector<uint8_t> m_payload;

};

#endif //BABLE_LINUX_HCIFORMATEXTRACTOR_HPP