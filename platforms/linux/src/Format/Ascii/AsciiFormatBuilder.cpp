#include "AsciiFormatBuilder.hpp"

using namespace std;

// Constructors
AsciiFormatBuilder::AsciiFormatBuilder() {
  m_name = "Generic";
};

// Setters
AsciiFormatBuilder& AsciiFormatBuilder::set_name(const string& name) {
  m_name = name;
}

// To add data to current building object
AsciiFormatBuilder& AsciiFormatBuilder::add(const string& field_name, const string& value) {
  stringstream stream;
  stream << field_name << ": " << value;

  m_data_vector.push_back(stream.str());

  return *this;
}

AsciiFormatBuilder& AsciiFormatBuilder::add(const string& field_name, const vector<string>& container) {
  stringstream stream;
  stream << field_name << ": [";

  for(auto it = container.begin(); it != container.end(); ++it) {
    stream << *it;
    if (next(it) != container.end()) {
      stream << ", ";
    }
  }

  stream << "]";

  m_data_vector.push_back(stream.str());

  return *this;
}

// To build and finish the result object
vector<uint8_t> AsciiFormatBuilder::build() {
  vector<uint8_t> result = generate_header();
  result.push_back(static_cast<uint8_t>('{'));

  for(auto it = m_data_vector.begin(); it != m_data_vector.end(); ++it) {

    result.reserve(result.size() + it->size() + 1);
    for (const char& c : *it) {
      result.push_back(static_cast<uint8_t>(c));
    }

    if (next(it) != m_data_vector.end()) {
      result.push_back(static_cast<uint8_t>(','));
    }
  }

  result.push_back(static_cast<uint8_t>('}'));

  return result;
}

// Private
vector<uint8_t> AsciiFormatBuilder::generate_header() {
  vector<uint8_t> header;
  header.reserve(m_name.size() + 2);

  header.push_back(static_cast<uint8_t>('<'));

  for (const char& c : m_name) {
    header.push_back(static_cast<uint8_t>(c));
  }

  header.push_back(static_cast<uint8_t>('>'));

  return header;
}
