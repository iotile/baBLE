#include "Deserializer.hpp"

using namespace std;

Deserializer::Deserializer() = default;

Deserializer::Deserializer(Deserializer& other) {
  import(other.m_buffer);
}

Deserializer::Deserializer(const std::vector<uint8_t>& buffer) {
  import(buffer);
}

Deserializer::Deserializer(const uint8_t* buffer, size_t size) {
  import(buffer, size);
}

Deserializer& Deserializer::operator=(const Deserializer& other) {
  if (&other == this) {
    return *this;
  }
  import(other.m_buffer);

  return *this;
}

size_t Deserializer::size() const {
  return m_buffer.size();
}

const uint8_t* Deserializer::buffer() const {
  return m_buffer.data();
}

void Deserializer::import(const uint8_t* buffer, size_t size) {
  size_t nb_elements = size / sizeof(buffer[0]);
  m_buffer.clear();
  m_buffer.resize(nb_elements);
  reverse_copy(buffer, buffer + nb_elements, m_buffer.begin());
}

void Deserializer::import(const vector<uint8_t>& buffer) {
  m_buffer.clear();
  m_buffer = vector<uint8_t>(buffer.rbegin(), buffer.rend());
}

Deserializer& Deserializer::operator>>(string& value) {
  for (vector<uint8_t>::reverse_iterator it = m_buffer.rbegin(); it != m_buffer.rend(); ++it) {
    if (*it == '\0') {
      m_buffer.pop_back();
      break;
    } else {
      value += *it;
      m_buffer.pop_back();
    }
  }

  return *this;
}

ostream& operator<<(ostream& output, const Deserializer& self) {
  output << "<Deserializer[" << self.size() << "]> { ";

  for (const uint8_t value : self.m_buffer) {
    output << HEX(value) << " ";
  }

  output << "}" << endl;

  return output;
}
