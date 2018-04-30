#ifndef BABLE_LINUX_MGMTFORMATBUILDER_HPP
#define BABLE_LINUX_MGMTFORMATBUILDER_HPP

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

#include <array>
#include <cstdint>
#include <vector>
#include <string>

class MGMTFormatBuilder {

public:
  MGMTFormatBuilder(uint16_t code, uint16_t controller_id) {
    m_code = code;
    m_controller_id = controller_id;
  };

  template<typename T>
  MGMTFormatBuilder& add(const T& value) {
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
  };

  MGMTFormatBuilder& add(const std::string& value) {
    for (const char& c : value) {
      this->add<char>(c);
    }
    this->add<char>('\0');

    return *this;
  };

  template<typename T, size_t N>
  MGMTFormatBuilder& add(const std::array<T,N>& container) {
    m_formatted_data.reserve(m_formatted_data.size() + N);
    for(const T& value : container) {
      this->add<T>(value);
    }

    return *this;
  };

  template<typename T>
  MGMTFormatBuilder& add(const std::vector<T>& container) {
    m_formatted_data.reserve(m_formatted_data.size() + container.size());
    for(const T& value : container) {
      this->add<T>(value);
    }

    return *this;
  };

  template<typename T, size_t N>
  MGMTFormatBuilder& add(const T (&container)[N]) {
    for(size_t i = 0; i < N; i++) {
      this->add<T>(container[i]);
    }

    return *this;
  };

  std::vector<uint8_t> build() {
    std::vector<uint8_t> result = generate_header();

    result.insert(result.end(), m_formatted_data.begin(), m_formatted_data.end());

    return result;
  };

private:
  std::vector<uint8_t> generate_header() {
    auto params_length = static_cast<uint16_t>(m_formatted_data.size());

    std::vector<uint8_t> header;
    header.reserve(6);

    // Use little-endian
    header.push_back(static_cast<uint8_t>(m_code & 0x00FF));
    header.push_back(static_cast<uint8_t>(m_code >> 8));

    header.push_back(static_cast<uint8_t>(m_controller_id & 0x00FF));
    header.push_back(static_cast<uint8_t>(m_controller_id >> 8));

    header.push_back(static_cast<uint8_t>(params_length & 0x00FF));
    header.push_back(static_cast<uint8_t>(params_length >> 8));

    return header;
  };

  uint16_t m_code;
  uint16_t m_controller_id;
  std::vector<uint8_t> m_formatted_data;

};

#endif //BABLE_LINUX_MGMTFORMATBUILDER_HPP
