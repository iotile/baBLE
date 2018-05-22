#include "MGMTFormatExtractor.hpp"

using namespace std;

// Static
uint16_t MGMTFormatExtractor::extract_type_code(const vector<uint8_t>& data) {
  if (data.size() < 2) {
    throw Exceptions::WrongFormatException("Given MGMT data are too small (< 2 bytes). Can't extract type code.");
  }

  // Use little endian
  uint16_t event_code = (data.at(1) << 8) | data.at(0);

  return event_code;
};

uint16_t MGMTFormatExtractor::extract_packet_code(const vector<uint8_t>& data, bool isRequest) {
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

    uint16_t event_code = extract_type_code(data);

    if (event_code != Format::MGMT::EventCode::CommandComplete && event_code != Format::MGMT::EventCode::CommandStatus) {
      throw Exceptions::WrongFormatException("Can't extract command code from given MGMT data: no command code inside");
    }

    // Use little endian
    command_code = (data.at(7) << 8) | data.at(6);
  }

  return command_code;
};

uint16_t MGMTFormatExtractor::extract_controller_id(const vector<uint8_t>& data) {
  uint16_t controller_id;

  if (data.size() < 4) {
    throw Exceptions::WrongFormatException("Given MGMT data are too small (< 4 bytes). Can't extract controller id.");
  }

  // Use little endian
  controller_id = (data.at(3) << 8) | data.at(2);

  return controller_id;
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
    : AbstractExtractor(data) {
  m_header_length = Format::MGMT::header_length;
  parse_header(data);
  set_data_pointer(m_header_length);

  if (m_type_code == Format::MGMT::EventCode::CommandComplete || m_type_code == Format::MGMT::EventCode::CommandStatus) {
    // Use little endian
    m_packet_code = get_value<uint16_t>();
  } else {
    m_packet_code = m_type_code;
  }
};

// Parsers
void MGMTFormatExtractor::parse_header(const vector<uint8_t>& data) {
  if (data.size() < Format::MGMT::header_length) {
    throw Exceptions::WrongFormatException("Given MGMT data are too small (< 6 bytes). Can't parse header.");
  }

  m_type_code = (static_cast<uint16_t>(data.at(1)) << 8) | data.at(0);
  m_controller_id = (static_cast<uint16_t>(data.at(3)) << 8) | data.at(2);
  m_data_length = (static_cast<uint16_t>(data.at(5)) << 8) | data.at(4);
};

Format::MGMT::EIR MGMTFormatExtractor::parse_eir(const vector<uint8_t>& data) {
  if (data.size() < 3) {
    throw Exceptions::WrongFormatException("Given EIR data are too small. Can't extract anything from it.");
  }

  Format::MGMT::EIR result = {};

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
