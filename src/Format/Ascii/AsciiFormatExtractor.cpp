#include "AsciiFormatExtractor.hpp"

using namespace std;

// Statics
uint16_t AsciiFormatExtractor::extract_type_code(const vector<uint8_t>& data) {
  return 0;
}

uint16_t AsciiFormatExtractor::extract_packet_code(const vector<uint8_t>& data) {
  if (data.empty()) {
    throw Exceptions::WrongFormatException("Given ASCII data are too small (< 1 bytes). Can't extract command code.");
  }

  string data_str;
  for (auto& value : data) {
    auto c = static_cast<char>(value);

    if (c == Format::Ascii::Delimiter) {
      break;
    }

    data_str += c;
  }

  return static_cast<uint16_t>(stoi(data_str));
}

uint16_t AsciiFormatExtractor::extract_controller_id(const vector<uint8_t>& data) {
  if (data.size() < 2) {
    throw Exceptions::WrongFormatException("Given ASCII data are too small (< 2 bytes). Can't extract controller id.");
  }

  string data_str;
  size_t nb_delimiters = 0;
  for (auto& value : data) {
    auto c = static_cast<char>(value);

    if (c == Format::Ascii::Delimiter) {
      nb_delimiters++;

      if (nb_delimiters > 1) {
        break;
      }
    }

    if (nb_delimiters > 0) {
      data_str += c;
    }
  }

  return static_cast<uint16_t>(stoi(data_str));
}

uint16_t AsciiFormatExtractor::extract_payload_length(const vector<uint8_t>& data) {
  throw std::runtime_error("Ascii format can't extract payload length: this information is not included in the format.");
}

// Constructors
AsciiFormatExtractor::AsciiFormatExtractor(const vector<uint8_t>& data) : AbstractExtractor(data) {
  parse_header(data);
}

// Parser
void AsciiFormatExtractor::parse_header(const std::vector<uint8_t>& data) {
  try {
    m_uuid_request = get_string();
  } catch (const Exceptions::WrongFormatException& err) {
    throw Exceptions::InvalidCommandException("Invalid command. Usage: <uuid>,<command_code>,<controller_id>,<params...>");
  }

  try {
    m_packet_code = static_cast<uint16_t>(stoi(get_string()));
    m_controller_id = static_cast<uint16_t>(stoi(get_string()));

  } catch (const Exceptions::WrongFormatException& err) {
    throw Exceptions::InvalidCommandException("Invalid command. Usage: <command_code>,<controller_id>,<params...>", m_uuid_request);
  } catch (const bad_cast& err) {
    throw Exceptions::InvalidCommandException("<command_code> and <controller_id> must be 16bits numbers.", m_uuid_request);
  } catch (const std::invalid_argument& err) {
    throw Exceptions::InvalidCommandException("<command_code> and <controller_id> must be numbers.", m_uuid_request);
  }
}

// Getters
string AsciiFormatExtractor::get_string() {
  string result;

  if (m_data_iterator >= m_data.end()) {
    throw Exceptions::WrongFormatException("No more string to extract from given data.");
  }

  while (m_data_iterator != m_data.end()) {
    auto c = get_value<char>();

    if (c == Format::Ascii::Delimiter) {
      break;
    }

    result += c;
  }

  return result;
}
