#ifndef BABLE_LINUX_SERIALIZER_HPP
#define BABLE_LINUX_SERIALIZER_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "utils/stream_formats.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

class Serializer {

public:
  Serializer();
  Serializer(Serializer& other);
  Serializer(const uint8_t* buffer, size_t size);
  explicit Serializer(const std::vector<uint8_t>& buffer);
  Serializer& operator=(const Serializer& other);

  size_t size() const;
  const uint8_t* buffer() const;

  void import(const uint8_t* buffer, size_t size);
  void import(const std::vector<uint8_t>& buffer);

  template<typename T>
  Serializer& operator<<(const T& value);

  Serializer& operator<<(const std::string& value);

  template<typename T, size_t N>
  Serializer& operator<<(const std::array<T,N>& container);

  template<typename T>
  Serializer& operator<<(const std::vector<T>& container);

  friend std::ostream& operator<<(std::ostream& output,const Serializer& self);

private:
  std::vector<uint8_t> m_buffer;

};

template<typename T>
Serializer& Serializer::operator<<(const T& value) {
  const size_t nb_bytes = sizeof(value);

  if (nb_bytes == 1) {
    m_buffer.push_back(value);

  } else {
    uint8_t* byte_ptr = (uint8_t*)&value;
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

#endif //BABLE_LINUX_SERIALIZER_HPP

/* Using
 *
 * Serializer ser;
 * uint16_t command_code;
 * uint16_t controller_id;
 * vector<uint8_t> params;
 * uint16_t param_length = params.size();
 * ser << command_code << controller_id << param_length;
 *
 * if (param_length > 0) {
 *    ser << params;
 * }
 *
 * send(ser.buffer(), ...);
 *
 *
 * char response[50]; OR vector<uint8_t> vec_response(1024)
 * read(sock, vec_response.data(), vec_response.size())
 *
 * uint8_t address[6];
   uint8_t bt_version;
   uint16_t manufacturer;
   uint32_t supported_settings;
   uint8_t device_class[3];
 *
 * Serializer deser(response); OR Serializer ser(vec_response);
 * deser >> address
 *       >> bt_version
 *       >> manufacturer
 *       >> supported_settings
 *       >> device_class;
 */
