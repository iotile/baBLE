#ifndef BABLE_LINUX_HCIFORMATEXTRACTOR_HPP
#define BABLE_LINUX_HCIFORMATEXTRACTOR_HPP

#include <array>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <vector>
#include "./constants.hpp"
#include "../AbstractExtractor.hpp"
#include "../../Log/Log.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

class HCIFormatExtractor : public AbstractExtractor {

public:
  static uint8_t extract_type_code(const std::vector<uint8_t>& data) {
    if (data.empty()) {
      throw Exceptions::WrongFormatException("Given HCI data are too small (< 1 bytes). Can't extract type code.");
    }

    uint8_t type_code = data.at(0);

    return type_code;
  };

  static uint16_t extract_payload_length(const std::vector<uint8_t>& data) {
    uint16_t payload_length;
    uint8_t type_code = extract_type_code(data);

    const size_t header_length = get_header_length(type_code);
    if (data.size() < header_length) {
      throw Exceptions::WrongFormatException("Given HCI data are too small (< " + std::to_string(header_length)
                                                 + " bytes). Can't extract payload length.");
    }

    switch (type_code) {
      case Format::HCI::Type::Command:
        payload_length = static_cast<uint16_t>(data.at(3));
        break;

      case Format::HCI::Type::AsyncData:
        // Use little endian
        payload_length = (data.at(6) << 8) | data.at(5);
        if (payload_length > 0) {
          payload_length -= 1; // To consider attribute opcode as part of the header
        }
        break;

      case Format::HCI::Type::Event:
        payload_length = static_cast<uint16_t>(data.at(2));
        break;

      default:
        throw Exceptions::WrongFormatException("Can't extract payload length from given HCI data: unknown type code.");
    }

    return payload_length;
  };

  static const size_t get_header_length(uint16_t type_code) {
    size_t header_length;

    switch (type_code) {
      case Format::HCI::Type::Command:
        header_length = Format::HCI::command_header_length;
        break;

      case Format::HCI::Type::AsyncData:
        header_length = Format::HCI::async_data_header_length;

        break;

      case Format::HCI::Type::Event:
        header_length = Format::HCI::event_header_length;
        break;

      default:
        throw Exceptions::WrongFormatException("Can't get header length from given type code: unknown type code.");
    }

    return header_length;
  };

  // Constructors
  explicit HCIFormatExtractor(const std::vector<uint8_t>& data)
    : AbstractExtractor(data) {
    parse_header(data);
    set_data_pointer(m_header_length);
  };

private:
  // Parsers
  void parse_header(const std::vector<uint8_t>& data) override {
    m_type_code = extract_type_code(data);
    m_header_length = get_header_length(m_type_code);
    if (data.size() < m_header_length) {
      throw Exceptions::WrongFormatException("Given HCI data are too small (< " + std::to_string(m_header_length)
                                                 + " bytes). Can't parse header.");
    }

    switch (m_type_code) {
      case Format::HCI::Type::Command:
        // Use little endian
        m_packet_code = (data.at(2) << 8) | data.at(1);
        m_data_length = static_cast<uint16_t>(data.at(3));
        break;

      case Format::HCI::Type::AsyncData:
        // Use little endian
        m_connection_id = (static_cast<uint16_t>(data.at(2) & 0x0F) << 8) | data.at(1);
        m_data_length = ((data.at(6) << 8) | data.at(5));
        if (m_data_length > 0) {
          m_data_length -= 1; // To consider opcode as part of the header
        }
        m_packet_code = static_cast<uint16_t>(data.at(9));
        break;

      case Format::HCI::Type::Event:
        {
          m_data_length = data.at(2);
          uint8_t event_code = data.at(1);
          if (event_code == Format::HCI::EventCode::LEMeta && data.size() >= m_header_length + 1) {
            m_packet_code = event_code << 8 | data.at(3);
            m_header_length += 1;
            m_data_length -= 1;
          } else {
            m_packet_code = event_code;
          }
        }
        break;

      default:
        throw Exceptions::WrongFormatException("Can't extract data length from given HCI data: unknown type code.");
    }
  };

};

#endif //BABLE_LINUX_HCIFORMATEXTRACTOR_HPP