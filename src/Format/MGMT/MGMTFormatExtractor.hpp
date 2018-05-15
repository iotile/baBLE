#ifndef BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP
#define BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP

#include <array>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <vector>
#include "./constants.hpp"
#include "../../Log/Log.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

class MGMTFormatExtractor {

public:
  static uint16_t extract_event_code(const std::vector<uint8_t>& data);
  static uint16_t extract_command_code(const std::vector<uint8_t>& data, bool isRequest = false);
  static uint16_t extract_controller_id(const std::vector<uint8_t>& data);
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data);

  // Constructors
  explicit MGMTFormatExtractor(const std::vector<uint8_t>& data);

  // Getters
  template<typename T>
  T get_value();
  template<typename T, size_t N>
  std::array<T, N> get_array();
  template<typename T>
  std::vector<T> get_vector(size_t length);

  inline uint16_t get_event_code() const {
    return m_event_code;
  }

  inline uint16_t get_controller_id() const {
    return m_controller_id;
  }

  // Parsers
  void parse_header(const std::vector<uint8_t>& data);
  Format::MGMT::EIR parse_eir(const std::vector<uint8_t>& data);

private:
  uint16_t m_event_code;
  uint16_t m_controller_id;
  uint16_t m_params_length;

  std::vector<uint8_t> m_payload;

};

template<typename T>
T MGMTFormatExtractor::get_value() {
  const size_t nb_bytes = sizeof(T);

  if (nb_bytes > m_payload.size()) {
    throw Exceptions::WrongFormatException("Can't deserialize given type: not enough bytes in payload.");
  }

  T result;
  if (nb_bytes == 1) {
    result = m_payload.back();
    m_payload.pop_back();

  } else {
    auto byte_ptr = (uint8_t*)&result;
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      for(size_t i = 0; i < nb_bytes; i++) {
        byte_ptr[i] = m_payload.back();
        m_payload.pop_back();
      }

    } else {
      for(size_t i = nb_bytes - 1; i >= 0; i--) {
        byte_ptr[i] = m_payload.back();
        m_payload.pop_back();
      }
    }
  }

  return result;
};

template<typename T, size_t N>
std::array<T, N> MGMTFormatExtractor::get_array() {
  std::array<T, N> result;

  for (auto& value : result) {
    value = get_value<T>();
  }

  return result;
};

template<typename T>
std::vector<T> MGMTFormatExtractor::get_vector(size_t length) {
  std::vector<T> result(length);

  for (auto& value : result) {
    value = get_value<T>();
  }

  return result;
};

#endif //BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP
