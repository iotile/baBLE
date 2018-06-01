#include "MGMTFormatExtractor.hpp"
#include "../../Log/Log.hpp"

using namespace std;

// Static
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
  if (data.size() < m_header_length) {
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
      case Format::MGMT::EIRType::Flags:
        result.flags = *it;
        break;

      case Format::MGMT::EIRType::IncompleteUUID16ServiceClass:
      case Format::MGMT::EIRType::UUID16ServiceClass:
        result.uuid.assign(it, it + 4);
        break;

      case Format::MGMT::EIRType::IncompleteUUID32ServiceClass:
      case Format::MGMT::EIRType::UUID32ServiceClass:
        result.uuid.assign(it, it + 8);
        break;

      case Format::MGMT::EIRType::IncompleteUUID128ServiceClass:
      case Format::MGMT::EIRType::UUID128ServiceClass:
        result.uuid.assign(it, it + 16);
        break;

      case Format::MGMT::EIRType::ManufacturerSpecific:
      {
        result.company_id = (*it << 8) | *(it + 1);
        field_length -= sizeof(result.company_id);
        it += sizeof(result.company_id);

        result.manufacturer_data.reserve(field_length);
        for (uint8_t i = 0; i < field_length; i ++) {
          result.manufacturer_data.push_back(*(it + i));
        }
        break;
      }

      case Format::MGMT::EIRType::CompleteDeviceName:
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
