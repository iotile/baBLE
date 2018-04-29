#ifndef BABLE_LINUX_SERIALIZER_HPP
#define BABLE_LINUX_SERIALIZER_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "AbstractSerializer.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

class Serializer : public AbstractSerializer {

public:
  Serializer() = default;
  explicit Serializer(const std::vector<uint8_t>& buffer) {
    import(buffer);
  };
  Serializer(const uint8_t* buffer, size_t size) {
    import(buffer, size);
  };

  void import(const uint8_t* buffer, size_t size) override;
  void import(const std::vector<uint8_t>& buffer) override;

  template<typename T>
  Serializer& operator<<(const T& value);

  Serializer& operator<<(const std::string& value);

  template<typename T, size_t N>
  Serializer& operator<<(const std::array<T,N>& container);

  template<typename T>
  Serializer& operator<<(const std::vector<T>& container);

  template<std::size_t N>
  Serializer& operator<<(const char (&container)[N]);

  Serializer& operator<<(const Serializer& other);

};

template<typename T>
Serializer& Serializer::operator<<(const T& value) {
  const size_t nb_bytes = sizeof(value);

  if (nb_bytes == 1) {
    m_buffer.push_back(value);

  } else {
    auto byte_ptr = (uint8_t*)&value;
    m_buffer.reserve(m_buffer.size() + nb_bytes);
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      for(size_t i = 0; i < nb_bytes; i++) {
        m_buffer.push_back(byte_ptr[i]);
      }

    } else {
      for(size_t i = nb_bytes - 1; i >= 0; i--) {
        m_buffer.push_back(byte_ptr[i]);
      }
    }
  }

  return *this;
}

template<typename T, size_t N>
Serializer& Serializer::operator<<(const std::array<T,N>& container) {
  m_buffer.reserve(m_buffer.size() + N);
  for(const T& value : container) {
    *this << value;
  }

  return *this;
}

template<typename T>
Serializer& Serializer::operator<<(const std::vector<T>& container) {
  m_buffer.reserve(m_buffer.size() + container.size());
  for(const T& value : container) {
    *this << value;
  }

  return *this;
}

template<std::size_t N>
Serializer& Serializer::operator<<(const char (&container)[N]) {
  for(size_t i = 0; i < N; i++) {
    *this << container[i];
  }

  return *this;
}

#endif //BABLE_LINUX_SERIALIZER_HPP
