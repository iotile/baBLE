#include "AsciiFormatExtractor.hpp"

using namespace std;

// Statics
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
