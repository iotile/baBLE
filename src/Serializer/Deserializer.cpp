#include "Deserializer.hpp"

using namespace std;

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
  for (auto it = m_buffer.rbegin(); it != m_buffer.rend(); ++it) {
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

Deserializer& Deserializer::operator>>(const Deserializer& other) {
  m_buffer.insert(m_buffer.end(), other.m_buffer.begin(), other.m_buffer.end());

  return *this;
}
