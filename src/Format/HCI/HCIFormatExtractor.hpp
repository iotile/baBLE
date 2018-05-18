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

  static uint16_t extract_packet_code(const std::vector<uint8_t>& data) {
    uint16_t packet_code;
    uint8_t type_code = extract_type_code(data);
    size_t header_length = get_header_length(type_code);

    if (data.size() < header_length) {
      throw Exceptions::WrongFormatException("Given HCI data are too small (< " + std::to_string(header_length)
                                                 + " bytes). Can't extract command code.");
    }

    switch (type_code) {
      case Format::HCI::Type::Command:
        // Use little endian
        packet_code = (data.at(2) << 8) | data.at(1);
        break;

      case Format::HCI::Type::AsyncData:
        packet_code = static_cast<uint16_t>(data.at(9));
        break;

      case Format::HCI::Type::Event:
        {
          uint8_t event_code = data.at(1);
          if (event_code == Format::HCI::EventCode::LEMeta && data.size() >= header_length + 1) {
            packet_code = event_code << 8 | data.at(3);
          } else {
            packet_code = static_cast<uint16_t>(event_code);
          }
        }
        break;

      default:
        throw Exceptions::WrongFormatException("Can't extract command code from given HCI data: unknown type code.");
    }

    return packet_code;
  };

  static uint16_t extract_controller_id(const std::vector<uint8_t>& data) {
    // TODO: is connection handle really equivalent to controller id ??? -> no
    uint16_t controller_id;
    uint8_t type_code = extract_type_code(data);

    if (type_code != Format::HCI::Type::AsyncData) {
      return NON_CONTROLLER_ID;
    }

    if (data.size() < Format::HCI::async_data_header_length) {
      throw Exceptions::WrongFormatException("Given HCI data are too small (< "
                                                 + std::to_string(Format::HCI::async_data_header_length)
                                                 + " bytes). Can't extract controller id.");
    }

    // Use little endian
    controller_id = (static_cast<uint16_t>(data.at(2) & 0x0F) << 8) | data.at(1);

    return controller_id;
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
        break;

      case Format::HCI::Type::Event:
        payload_length = static_cast<uint16_t>(data.at(2));
        break;

      default:
        throw Exceptions::WrongFormatException("Can't extract payload length from given HCI data: unknown type code.");
    }

    return payload_length;
  };

  static const size_t get_header_length(uint8_t type_code) {
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
    : m_type_code(0), m_params_length(0), m_command_code(0), m_connection_handle(0), m_attribute_code(0),
      m_full_event_code(0), m_header_length(0) {
    parse_header(data);
    m_payload.assign(data.rbegin(), data.rend() - m_header_length);
  };

  // Getters
  template<typename T>
  T get_value();
  template<typename T, size_t N>
  std::array<T, N> get_array();
  template<typename T>
  std::vector<T> get_vector(size_t length);

private:
  // Parsers
  void parse_header(const std::vector<uint8_t>& data) {
    m_type_code = extract_type_code(data);
    m_header_length = get_header_length(m_type_code);
    if (data.size() < m_header_length) {
      throw Exceptions::WrongFormatException("Given HCI data are too small (< " + std::to_string(m_header_length)
                                                 + " bytes). Can't parse header.");
    }

    switch (m_type_code) {
      case Format::HCI::Type::Command:
        // Use little endian
        m_command_code = (data.at(2) << 8) | data.at(1);
        m_params_length = static_cast<uint16_t>(data.at(3));
        break;

      case Format::HCI::Type::AsyncData:
        // Use little endian
        m_connection_handle = (static_cast<uint16_t>(data.at(2) & 0x0F) << 8) | data.at(1);
        m_params_length = ((data.at(6) << 8) | data.at(5));
        if (m_params_length > 0) {
          m_params_length -= 1;
        }
        m_attribute_code = data.at(9);
        break;

      case Format::HCI::Type::Event:
        {
          uint8_t event_code = data.at(1);
          if (event_code == Format::HCI::EventCode::LEMeta && data.size() >= m_header_length + 1) {
            m_full_event_code = event_code << 8 | data.at(3);
            m_header_length += 1;
          }
          m_params_length = data.at(2);

        }
        break;

      default:
        throw Exceptions::WrongFormatException("Can't extract data length from given HCI data: unknown type code.");
    }
  };

  uint8_t m_type_code;
  uint16_t m_params_length;

  // Command
  uint16_t m_command_code;
  // Data
  uint16_t m_connection_handle;
  uint8_t m_attribute_code;
  // Event
  uint16_t m_full_event_code;
  size_t m_header_length;

  std::vector<uint8_t> m_payload;

};

template<typename T>
T HCIFormatExtractor::get_value() {
  const size_t nb_bytes = sizeof(T);

  if (nb_bytes > m_payload.size()) {
    throw Exceptions::WrongFormatException("Can't deserialize given type: not enough bytes in payload.");
  }

  T result;
  if (nb_bytes == 1) {
    result = m_payload.back();
    m_payload.pop_back();

  } else {
    auto byte_ptr = (uint8_t*)&result;
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      for(size_t i = 0; i < nb_bytes; i++) {
        byte_ptr[i] = m_payload.back();
        m_payload.pop_back();
      }

    } else {
      for(size_t i = nb_bytes - 1; i >= 0; i--) {
        byte_ptr[i] = m_payload.back();
        m_payload.pop_back();
      }
    }
  }

  return result;
};

template<typename T, size_t N>
std::array<T, N> HCIFormatExtractor::get_array() {
  std::array<T, N> result;

  for (auto& value : result) {
    value = get_value<T>();
  }

  return result;
};

template<typename T>
std::vector<T> HCIFormatExtractor::get_vector(size_t length) {
  std::vector<T> result(length);

  for (auto& value : result) {
    value = get_value<T>();
  }

  return result;
};

#endif //BABLE_LINUX_HCIFORMATEXTRACTOR_HPP