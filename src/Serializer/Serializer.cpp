#include "Serializer.hpp"

using namespace std;

Serializer::Serializer() = default;

Serializer::Serializer(Serializer& other) {
  import(other.m_buffer);
}

Serializer::Serializer(const std::vector<uint8_t>& buffer) {
  import(buffer);
}

Serializer::Serializer(const uint8_t* buffer, size_t size) {
  import(buffer, size);
}

void Serializer::import(const uint8_t* buffer, size_t size) {
  m_buffer.clear();
  m_buffer = vector<uint8_t>(buffer, buffer + size / sizeof(buffer[0]));
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
