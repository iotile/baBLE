#include "MGMTFormatBuilder.hpp"

using namespace std;

// Constructors
MGMTFormatBuilder::MGMTFormatBuilder(uint16_t controller_id) {
  m_code = 0;
  m_controller_id = controller_id;
};

// Setters
MGMTFormatBuilder& MGMTFormatBuilder::set_code(uint16_t code) {
  m_code = code;
  return *this;
}

// To add data to current building object
MGMTFormatBuilder& MGMTFormatBuilder::add(const string& value) {
  for (const char& c : value) {
    this->add<char>(c);
  }
  this->add<char>('\0');

  return *this;
};

// To build and finish the result object
vector<uint8_t> MGMTFormatBuilder::build() {
  vector<uint8_t> result = generate_header();

  result.insert(result.end(), m_formatted_data.begin(), m_formatted_data.end());

  return result;
};

// Private
vector<uint8_t> MGMTFormatBuilder::generate_header() {
  auto params_length = static_cast<uint16_t>(m_formatted_data.size());

  vector<uint8_t> header;
  header.reserve(Format::MGMT::header_length);

  // Use little-endian
  header.push_back(static_cast<uint8_t>(m_code & 0x00FF));
  header.push_back(static_cast<uint8_t>(m_code >> 8));

  header.push_back(static_cast<uint8_t>(m_controller_id & 0x00FF));
  header.push_back(static_cast<uint8_t>(m_controller_id >> 8));

  header.push_back(static_cast<uint8_t>(params_length & 0x00FF));
  header.push_back(static_cast<uint8_t>(params_length >> 8));

  return header;
};
