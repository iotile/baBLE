#include "AsciiFormatExtractor.hpp"

using namespace std;

// Statics
uint16_t AsciiFormatExtractor::extract_command_code(const vector<uint8_t>& data) {
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

// Constructors
AsciiFormatExtractor::AsciiFormatExtractor(const vector<uint8_t>& data) {
  m_stack_pointer = 0;
  parse_payload(data);

  try {
    m_uuid_request = get();
  } catch (const Exceptions::WrongFormatException& err) {
    throw Exceptions::InvalidCommandException("Invalid command. Usage: <uuid>,<command_code>,<controller_id>,<params...>");
  }

  try {
    m_command_code = static_cast<uint16_t>(stoi(get()));
    m_controller_id = static_cast<uint16_t>(stoi(get()));

  } catch (const Exceptions::WrongFormatException& err) {
    throw Exceptions::InvalidCommandException("Invalid command. Usage: <command_code>,<controller_id>,<params...>", m_uuid_request);
  } catch (const bad_cast& err) {
    throw Exceptions::InvalidCommandException("<command_code> and <controller_id> must be 16bits numbers.", m_uuid_request);
  } catch (const std::invalid_argument& err) {
    throw Exceptions::InvalidCommandException("<command_code> and <controller_id> must be numbers.", m_uuid_request);
  }
}

// Parser
void AsciiFormatExtractor::parse_payload(const vector<uint8_t>& data) {
  string data_str;

  for (auto& value : data) {
    auto c = static_cast<char>(value);

    if (c == Format::Ascii::Delimiter) {
      m_payload.push_back(data_str);
      data_str.clear();
      continue;
    }

    data_str += c;
  }

  m_payload.push_back(data_str);
}

// Getters
string AsciiFormatExtractor::get() {
  if (m_stack_pointer >= m_payload.size()) {
    throw Exceptions::WrongFormatException("No more data to get from AsciiFormatExtractor");
  }

  string result = m_payload.at(m_stack_pointer);
  m_stack_pointer++;

  return result;
}
