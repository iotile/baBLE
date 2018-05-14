#include "MGMTFormatExtractor.hpp"

using namespace std;

// Static
uint16_t MGMTFormatExtractor::extract_event_code(const vector<uint8_t>& data) {
  if (data.size() < 2) {
    throw Exceptions::WrongFormatException("Given MGMT data are too small (< 2 bytes). Can't extract event code.");
  }

  // Use little endian
  uint16_t event_code = (data.at(1) << 8) | data.at(0);

  return event_code;
};

uint16_t MGMTFormatExtractor::extract_command_code(const vector<uint8_t>& data, bool isRequest) {
  uint16_t command_code;

  if (isRequest) {
    if (data.size() < 2) {
      throw Exceptions::WrongFormatException("Given MGMT data are too small (< 2 bytes). Can't extract command code.");
    }

    // Use little endian
    command_code = (data.at(1) << 8) | data.at(0);

  } else {
    if (data.size() < 8) {
      throw Exceptions::WrongFormatException("Given MGMT data are too small (< 8 bytes). Can't extract command code.");
    }

    uint16_t event_code = extract_event_code(data);

    if (event_code != 0x0001 && event_code != 0x0002) {
      throw Exceptions::WrongFormatException("Can't extract command code from given MGMT data: no command code inside");
    }

    // Use little endian
    command_code = (data.at(7) << 8) | data.at(6);
  }

  return command_code;
};

uint16_t MGMTFormatExtractor::extract_payload_length(const vector<uint8_t>& data) {
  if (data.size() < 6) {
    throw Exceptions::WrongFormatException("Given MGMT data are too small (< 6 bytes). Can't extract payload length.");
  }

  // Use little endian
  uint16_t payload_length = (data.at(5) << 8) | data.at(4);
  return payload_length;
};

// Constructors
MGMTFormatExtractor::MGMTFormatExtractor(const vector<uint8_t>& data)
    : m_event_code(0), m_controller_id(0), m_params_length(0) {
  parse_header(data);
  m_payload.assign(data.rbegin(), data.rend() - 6);
};

// Parsers
void MGMTFormatExtractor::parse_header(const vector<uint8_t>& data) {
  if (data.size() < Format::MGMT::header_length) {
    throw Exceptions::WrongFormatException("Given MGMT data are too small (< 6 bytes). Can't parse header.");
  }

  m_event_code = (static_cast<uint16_t>(data.at(1)) << 8) | data.at(0);
  m_controller_id = (static_cast<uint16_t>(data.at(3)) << 8) | data.at(2);
  m_params_length = (static_cast<uint16_t>(data.at(5)) << 8) | data.at(4);
};

EIR MGMTFormatExtractor::parse_eir(const vector<uint8_t>& data) {
  if (data.size() < 3) {
    throw Exceptions::WrongFormatException("Given EIR data are too small. Can't extract anything from it.");
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
        LOG.warning("Unknown EIR type received: " + to_string(type), "DeviceFoundEvent");
        break;
    }

    for(uint8_t i = 0; i < field_length - 1; i++) {
      if(++it == data.end()) return result;
    }
  }

  return result;
};
