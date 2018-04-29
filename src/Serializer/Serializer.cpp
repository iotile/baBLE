#include "Serializer.hpp"

using namespace std;

void Serializer::import(const uint8_t* buffer, size_t size) {
  size_t nb_elements = size / sizeof(buffer[0]);
  m_buffer.clear();
  m_buffer = vector<uint8_t>(buffer, buffer + nb_elements);
}

void Serializer::import(const vector<uint8_t>& buffer) {
  m_buffer.clear();
  m_buffer = buffer;
}

Serializer& Serializer::operator<<(const string& value) {
  for (const char& c : value) {
    *this << c;
  }
  *this << '\0';

  return *this;
}

Serializer& Serializer::operator<<(const Serializer& other) {
  m_buffer.insert(m_buffer.end(), other.m_buffer.begin(), other.m_buffer.end());

  return *this;
}
