#ifndef BABLE_LINUX_DESERIALIZER_HPP
#define BABLE_LINUX_DESERIALIZER_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

#include "AbstractSerializer.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

class Deserializer : public AbstractSerializer {

public:
  void import(const uint8_t* buffer, size_t size) override;
  void import(const std::vector<uint8_t>& buffer) override;

  template<typename T>
  Deserializer& operator>>(T& value);

  Deserializer& operator>>(std::string& value);

  template<typename T, size_t N>
  Deserializer& operator>>(std::array<T,N>& container);

  template<typename T>
  Deserializer& operator>>(std::vector<T>& container);

};

template<typename T>
Deserializer& Deserializer::operator>>(T& value) {
  const size_t nb_bytes = sizeof(value);

  if (nb_bytes == 1) {
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
