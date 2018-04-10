#ifndef BABLE_LINUX_DESERIALIZER_HPP
#define BABLE_LINUX_DESERIALIZER_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

#include "utils/stream_formats.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

class Deserializer {

public:
  Deserializer();
  Deserializer(Deserializer& other);
  Deserializer(const uint8_t* buffer, size_t size);
  explicit Deserializer(const std::vector<uint8_t>& buffer);
  Deserializer& operator=(const Deserializer& other);

  size_t size() const;
  const uint8_t* buffer() const;

  void import(const uint8_t* buffer, size_t size);
  void import(const std::vector<uint8_t>& buffer);

  template<typename T>
  Deserializer& operator>>(T& value);

  Deserializer& operator>>(std::string& value);

  template<typename T, size_t N>
  Deserializer& operator>>(std::array<T,N>& container);

  template<typename T>
  Deserializer& operator>>(std::vector<T>& container);

  friend std::ostream& operator<<(std::ostream& output,const Deserializer& self);

private:
  std::vector<uint8_t> m_buffer;
};

template<typename T>
Deserializer& Deserializer::operator>>(T& value) {
  const size_t nb_bytes = sizeof(value);

  if (nb_bytes == 1) {
    // TODO: protect with mutex
    value = m_buffer.back();
    m_buffer.pop_back();
  } else {
    uint8_t* byte_ptr = (uint8_t*)&value;
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      for(size_t i = 0; i < nb_bytes; i++) {
        byte_ptr[i] = m_buffer.back();
        m_buffer.pop_back();
      }

    } else {
      for(size_t i = nb_bytes - 1; i >= 0; i--) {
        byte_ptr[i] = m_buffer.back();
        m_buffer.pop_back();
      }
    }
  }

  return *this;
}

template<typename T, size_t N>
Deserializer& Deserializer::operator>>(std::array<T,N>& container) {
  for(T& value : container) {
    *this >> value;
  }

  return *this;
}

template<typename T>
Deserializer& Deserializer::operator>>(std::vector<T>& container) {
  for(T& value : container) {
    *this >> value;
  }

  return *this;
}


#endif //BABLE_LINUX_DESERIALIZER_HPP
