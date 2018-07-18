#include "HCIFormatExtractor.hpp"
#include "Log/Log.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

// Statics
uint8_t HCIFormatExtractor::extract_type_code(const vector<uint8_t>& data) {
  if (data.empty()) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::WrongFormat,
        "Given HCI data are too small (< 1 bytes). Can't extract type code."
    );
  }

  uint8_t type_code = data.at(0);

  return type_code;
}

uint16_t HCIFormatExtractor::extract_payload_length(const vector<uint8_t>& data) {
  uint16_t payload_length;
  uint8_t type_code = extract_type_code(data);

  const size_t header_length = get_header_length(type_code);
  if (data.size() < header_length) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::WrongFormat,
        "Given HCI data are too small (< " + to_string(header_length) + " bytes). Can't extract payload length."
    );
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
      throw Exceptions::BaBLEException(
          BaBLE::StatusCode::WrongFormat,
          "Can't extract payload length from given HCI data: unknown type code."
      );
  }

  return payload_length;
}

const size_t HCIFormatExtractor::get_header_length(uint16_t type_code) {
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
      throw Exceptions::BaBLEException(
          BaBLE::StatusCode::WrongFormat,
          "Can't get header length from given type code: unknown type code."
      );
  }

  return header_length;
}

// Constructors
HCIFormatExtractor::HCIFormatExtractor(const vector<uint8_t>& data)
    : AbstractExtractor(data) {
  parse_header(data);
  set_data_pointer(m_header_length);
}

Format::HCI::EIR HCIFormatExtractor::parse_eir(const vector<uint8_t>& data) {
  if (data.size() < 3) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::WrongFormat,
        "Given EIR data are too small. Can't extract anything from it."
    );
  }

  Format::HCI::EIR result = {};

  for(auto it = data.begin(); it != data.end(); ++it) {
    uint8_t field_length = *it;
    if (field_length == 0) {
      // Sometimes AdvertisingReport has 0 bytes padding, meaning it is the end of advertising data
      break;
    }
    if(++it == data.end()) return result;

    uint8_t type = *it;
    if(++it == data.end()) return result;

    field_length--;

    // TODO: currently only read 1 UUID but multiple can be provided at once
    switch(type) {
      case Format::HCI::ReportType::Flags:
        result.flags = *it;
        break;

      case Format::HCI::ReportType::IncompleteUUID16ServiceClass:
      case Format::HCI::ReportType::UUID16ServiceClass:
        result.uuid.assign(it, it + 2);
        break;

      case Format::HCI::ReportType::IncompleteUUID32ServiceClass:
      case Format::HCI::ReportType::UUID32ServiceClass:
        result.uuid.assign(it, it + 4);
        break;

      case Format::HCI::ReportType::IncompleteUUID128ServiceClass:
      case Format::HCI::ReportType::UUID128ServiceClass:
        result.uuid.assign(it, it + 16);
        break;

      case Format::HCI::ReportType::UUID16ServiceData:
        result.uuid.assign(it, it + 2);
        // TODO: service data ignored
        break;

      case Format::HCI::ReportType::ManufacturerSpecific:
      {
        result.company_id = *(it + 1) << 8 | *it;
        field_length -= sizeof(result.company_id);
        it += sizeof(result.company_id);

        result.manufacturer_data.reserve(field_length);
        for (uint8_t i = 0; i < field_length; i ++) {
          result.manufacturer_data.push_back(*(it + i));
        }
        break;
      }

      case Format::HCI::ReportType::CompleteDeviceName:
        result.device_name.assign(it, it + field_length);
        break;

      case Format::HCI::ReportType::ShortDeviceName:
        if (result.device_name.empty()) {
          result.device_name.assign(it, it + field_length);
        }
        break;

      default:
        LOG.debug("Unknown EIR type received: " + to_string(type));
        break;
    }

    for(uint8_t i = 0; i < field_length - 1; i++) {
      if(++it == data.end()) return result;
    }
  }

  return result;
}

// Private

// Parsers
void HCIFormatExtractor::parse_header(const vector<uint8_t>& data) {
  m_type_code = extract_type_code(data);
  m_header_length = get_header_length(m_type_code);
  if (data.size() < m_header_length) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::WrongFormat,
        "Given HCI data are too small (< " + to_string(m_header_length) + " bytes). Can't parse header."
    );
  }

  switch (m_type_code) {
    case Format::HCI::Type::Command:
      // Use little endian
      m_packet_code = (data.at(2) << 8) | data.at(1);
      m_data_length = static_cast<uint16_t>(data.at(3));
      break;

    case Format::HCI::Type::AsyncData:
      // Use little endian
      m_connection_handle = (static_cast<uint16_t>(data.at(2) & 0x0F) << 8) | data.at(1);
      m_data_length = ((data.at(6) << 8) | data.at(5));
      if (m_data_length > 0) {
        m_data_length -= 1; // To consider opcode as part of the header
      }
      m_valid = static_cast<uint16_t>(data.at(8) << 8 | data.at(7)) == ATT_CID;
      m_packet_code = static_cast<uint16_t>(data.at(9));
      break;

    case Format::HCI::Type::Event:
    {
      m_data_length = data.at(2);
      uint8_t event_code = data.at(1);
      if (event_code == Format::HCI::EventCode::LEMeta && data.size() >= m_header_length + 1) {
        m_packet_code = event_code << 8 | data.at(3);
        m_header_length += 1; // To consider LEMeta subevent as part of the header...
        m_data_length -= 1; // ... and not part of the data
      } else {
        m_packet_code = event_code;
      }
    }
    break;

    default:
      throw Exceptions::BaBLEException(
          BaBLE::StatusCode::WrongFormat,
          "Can't extract data length from given HCI data: unknown type code."
      );
  }
};
