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

// Constructors
AsciiFormatExtractor::AsciiFormatExtractor(const vector<uint8_t>& data) {
  m_stack_pointer = 0;
  parse_payload(data);
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
