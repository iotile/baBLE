#ifndef BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP
#define BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP

#include <array>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <vector>
#include "../../Log/Log.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

struct EIR {
  uint8_t flags = 0;
  std::vector<uint8_t> uuid;
  uint16_t company_id = 0;
  std::vector<uint8_t> device_name;
};

class MGMTFormatExtractor {

public:
  static uint16_t extract_event_code(const std::vector<uint8_t>& data) {
    if (data.size() < 2) {
      throw std::invalid_argument("Given MGMT data are too small (< 2 bytes). Can't extract event code.");
    }

    // Use little endian
    uint16_t event_code = (data.at(1) << 8) | data.at(0);

    return event_code;
  };

  static uint16_t extract_command_code(const std::vector<uint8_t>& data, bool isRequest = false) {
    uint16_t command_code;

    if (isRequest) {
      if (data.size() < 2) {
        throw std::invalid_argument("Given MGMT data are too small (< 2 bytes). Can't extract command code.");
      }

      // Use little endian
      command_code = (data.at(1) << 8) | data.at(0);

    } else {
      if (data.size() < 8) {
        throw std::invalid_argument("Given MGMT data are too small (< 8 bytes). Can't extract command code.");
      }

      uint16_t event_code = extract_event_code(data);

      if (event_code != 0x0001 && event_code != 0x0002) {
        throw std::invalid_argument("Can't extract command code from given MGMT data: no command code inside");
      }

      // Use little endian
      command_code = (data.at(7) << 8) | data.at(6);
    }

    return command_code;
  };

  static uint16_t extract_payload_length(const std::vector<uint8_t>& data) {
    if (data.size() < 6) {
      throw std::invalid_argument("Given MGMT data are too small (< 6 bytes). Can't extract payload length.");
    }

    // Use little endian
    uint16_t payload_length = (data.at(5) << 8) | data.at(4);
    return payload_length;
  };

  explicit MGMTFormatExtractor(const std::vector<uint8_t>& data)
      : m_event_code(0), m_controller_id(0), m_params_length(0) {
    parse_header(data);
    m_payload.assign(data.rbegin(), data.rend() - 6);
  };

  void parse_header(const std::vector<uint8_t>& data) {
    if (data.size() < 6) {
      throw std::invalid_argument("Given MGMT data are too small (< 6 bytes). Can't parse header.");
    }

    m_event_code = (static_cast<uint16_t>(data.at(1)) << 8) | data.at(0);
    m_controller_id = (static_cast<uint16_t>(data.at(3)) << 8) | data.at(2);
    m_params_length = (static_cast<uint16_t>(data.at(5)) << 8) | data.at(4);
  };

  template<typename T>
  T get_value() {
    const size_t nb_bytes = sizeof(T);

    if (nb_bytes > m_payload.size()) {
      throw std::invalid_argument("Can't deserialize given type: not enough bytes in payload.");
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
  std::array<T, N> get_array() {
    std::array<T, N> result;

    for (auto& value : result) {
      value = get_value<T>();
    }

    return result;
  };

  template<typename T>
  std::vector<T> get_vector(size_t length) {
    std::vector<T> result(length);

    for (auto& value : result) {
      value = get_value<T>();
    }

    return result;
  };

  EIR parse_eir(const std::vector<uint8_t>& data) {
    if (data.size() < 3) {
      throw std::invalid_argument("Given EIR data are too small. Can't extract anything from it.");
    }

    EIR result = EIR();

    for(auto it = data.begin(); it != data.end(); ++it) {
      uint8_t field_length = *it;
      if(++it == data.end()) return result;

      uint8_t type = *it;
      if(++it == data.end()) return result;

      field_length--;

      switch(type) {
        case 0x01: // Flags
          result.flags = *it;
          break;

        case 0x02: // 16bits UUID
        case 0x03:
          result.uuid.assign(it, it + 4);
          break;

        case 0x04: // 32bits UUID
        case 0x05:
          result.uuid.assign(it, it + 8);
          break;

        case 0x06: // 128bits UUID
        case 0x07:
          result.uuid.assign(it, it + 16);
          break;

        case 0xFF: // Manufacturer Specific
          result.company_id = (*it << 8) | *(it + 1);
          break;

        case 0x09: // Device complete name
          result.device_name.assign(it, it + field_length);
          break;

        default:
          LOG.debug("Unknown EIR type received: " + std::to_string(type), "DeviceFoundEvent");
          break;
      }

      for(uint8_t i = 0; i < field_length - 1; i++) {
        if(++it == data.end()) return result;
      }
    }

    return result;
  };

  uint16_t get_event_code() {
    return m_event_code;
  }

  uint16_t get_controller_id() {
    return m_controller_id;
  }

private:
  uint16_t m_event_code;
  uint16_t m_controller_id;
  uint16_t m_params_length;
  std::vector<uint8_t> m_payload;

};

#endif //BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP
