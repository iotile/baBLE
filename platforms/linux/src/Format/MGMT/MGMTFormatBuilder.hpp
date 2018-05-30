#ifndef BABLE_LINUX_MGMTFORMATBUILDER_HPP
#define BABLE_LINUX_MGMTFORMATBUILDER_HPP

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include "./constants.hpp"

class MGMTFormatBuilder {

public:
  // Constructors
  explicit MGMTFormatBuilder(uint16_t controller_id);

  // Setters
  MGMTFormatBuilder& set_opcode(uint16_t opcode);
  MGMTFormatBuilder& set_controller_id(uint16_t controller_id);

  // To add data to current building object
  template<typename T>
  MGMTFormatBuilder& add(const T& value);
  MGMTFormatBuilder& add(const std::string& value);
  template<typename T, size_t N>
  MGMTFormatBuilder& add(const std::array<T,N>& container);
  template<typename T>
  MGMTFormatBuilder& add(const std::vector<T>& container);
  template<typename T, size_t N>
  MGMTFormatBuilder& add(const T (&container)[N]);

  // To build and finish the result object
  std::vector<uint8_t> build();

private:
  std::vector<uint8_t> generate_header();

  uint16_t m_opcode;
  uint16_t m_controller_id;
  std::vector<uint8_t> m_formatted_data;

};

template<typename T>
MGMTFormatBuilder& MGMTFormatBuilder::add(const T& value) {
  const size_t nb_bytes = sizeof(value);
  m_formatted_data.reserve(m_formatted_data.size() + nb_bytes);

  if (nb_bytes == 1) {
    m_formatted_data.push_back(value);

  } else {
    auto byte_ptr = (uint8_t*)&value;

    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      for(size_t i = 0; i < nb_bytes; i++) {
        m_formatted_data.push_back(byte_ptr[i]);
      }
    } else {
      for(size_t i = nb_bytes - 1; i >= 0; i--) {
        m_formatted_data.push_back(byte_ptr[i]);
      }
    }
  }

  return *this;
}

template<typename T, size_t N>
MGMTFormatBuilder& MGMTFormatBuilder::add(const std::array<T,N>& container) {
  m_formatted_data.reserve(m_formatted_data.size() + N);
  for(const T& value : container) {
    this->add<T>(value);
  }

  return *this;
}

template<typename T>
MGMTFormatBuilder& MGMTFormatBuilder::add(const std::vector<T>& container) {
  m_formatted_data.reserve(m_formatted_data.size() + container.size());
  for(const T& value : container) {
    this->add<T>(value);
  }

  return *this;
}

template<typename T, size_t N>
MGMTFormatBuilder& MGMTFormatBuilder::add(const T (&container)[N]) {
  for(size_t i = 0; i < N; i++) {
    this->add<T>(container[i]);
  }

  return *this;
}

#endif //BABLE_LINUX_MGMTFORMATBUILDER_HPP
