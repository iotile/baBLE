#ifndef BABLE_LINUX_EXTRACTOR_HPP
#define BABLE_LINUX_EXTRACTOR_HPP

#include <array>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <vector>
#include "../Exceptions/WrongFormat/WrongFormatException.hpp"
#include "HCI/constants.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

class AbstractExtractor {

public:
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data) {
    throw std::runtime_error("extract_payload_length() not implemented in AbstractExtractor.");
  };

  // Constructors
  explicit AbstractExtractor(const std::vector<uint8_t>& data) {
    m_type_code = 0;
    m_packet_code = 0;
    m_controller_id = NON_CONTROLLER_ID;
    m_connection_id = 0;
    m_header_length = 0;
    m_data.assign(data.begin(), data.end());
    m_data_length = static_cast<uint16_t>(m_data.size());
    m_data_iterator = m_data.begin();
  };

  // Getters
  template<typename T>
  T get_value();
  template<typename T, size_t N>
  std::array<T, N> get_array();
  template<typename T>
  std::vector<T> get_vector(size_t length);
  template<typename T>
  std::vector<T> get_vector();

  inline virtual std::vector<uint8_t> get_raw_data() const {
    return m_data;
  };

  inline virtual uint16_t get_type_code() const {
    return m_type_code;
  };

  inline virtual uint16_t get_packet_code() const {
    return m_packet_code;
  };

  inline virtual uint16_t get_controller_id() const {
    return m_controller_id;
  };

  inline virtual uint16_t get_connection_id() const {
    return m_connection_id;
  };

  const virtual uint16_t get_data_length() const {
    return m_data_length;
  };

  // Setters
  void set_data_pointer(size_t value) {
    if (value < 0 || value >= m_data.size()) {
      throw std::invalid_argument("Trying to set a new data pointer value which is out of bonds.");
    }

    m_data_iterator = m_data.begin() + value;
  };

  void set_controller_id(uint16_t controller_id) {
    m_controller_id = controller_id;
  };

protected:
  // Parsers
  virtual void parse_header(const std::vector<uint8_t>& data) {
    throw std::runtime_error("parse_header() function is not defined in AbstractExtractor.");
  };

  uint16_t m_type_code;
  uint16_t m_packet_code;
  uint16_t m_controller_id;
  uint16_t m_connection_id;
  uint16_t m_data_length;
  size_t m_header_length;

  std::vector<uint8_t>::iterator m_data_iterator;
  std::vector<uint8_t> m_data;

};

template<typename T>
T AbstractExtractor::get_value() {
  const size_t nb_bytes = sizeof(T);

  if (m_data_iterator + nb_bytes > m_data.end()) {
    throw Exceptions::WrongFormatException("Can't deserialize given type: not enough bytes in payload.");
  }

  T result;
  if (nb_bytes == 1) {
    result = *m_data_iterator;
    m_data_iterator++;

  } else {
    auto byte_ptr = (uint8_t*)&result;
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      for(size_t i = 0; i < nb_bytes; i++) {
        byte_ptr[i] = *m_data_iterator;
        m_data_iterator++;
      }

    } else {
      for(size_t i = nb_bytes - 1; i >= 0; i--) {
        byte_ptr[i] = *m_data_iterator;
        m_data_iterator++;
      }
    }
  }

  return result;
};

template<typename T, size_t N>
std::array<T, N> AbstractExtractor::get_array() {
  std::array<T, N> result;

  for (auto& value : result) {
    value = get_value<T>();
  }

  return result;
};

template<typename T>
std::vector<T> AbstractExtractor::get_vector(size_t length) {
  std::vector<T> result(length);

  for (auto& value : result) {
    value = get_value<T>();
  }

  return result;
};

template<typename T>
std::vector<T> AbstractExtractor::get_vector() {
  std::vector<T> result;

  while (true) {
    try {
      T value = get_value<T>();
      result.push_back(value);
    } catch (const Exceptions::WrongFormatException&) {
      break;
    }
  }

  return result;
};

#endif //BABLE_LINUX_EXTRACTOR_HPP
