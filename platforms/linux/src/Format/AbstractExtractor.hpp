#ifndef BABLE_LINUX_EXTRACTOR_HPP
#define BABLE_LINUX_EXTRACTOR_HPP

#include "./HCI/constants.hpp"

#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error "Byte order not suported (PDP endian)"
#endif

class AbstractExtractor {

public:
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data) {
    throw std::runtime_error("extract_payload_length() not implemented in AbstractExtractor.");
  };

  // Constructors
  explicit AbstractExtractor(const std::vector<uint8_t>& data);

  // Getters
  template<typename T>
  T get_value();

  template<typename T, size_t N>
  std::array<T, N> get_array();

  template<typename T>
  std::vector<T> get_vector(size_t length);

  template<typename T>
  std::vector<T> get_vector();

  inline std::vector<uint8_t> get_raw_data() const {
    return m_data;
  };

  inline uint16_t get_type_code() const {
    return m_type_code;
  };

  inline uint16_t get_packet_code() const {
    return m_packet_code;
  };

  inline uint16_t get_controller_id() const {
    return m_controller_id;
  };

  inline uint16_t get_connection_handle() const {
    return m_connection_handle;
  };

  inline const uint16_t get_data_length() const {
    return m_data_length;
  };

  // Setters
  void set_data_pointer(size_t value);
  void set_controller_id(uint16_t controller_id);

protected:
  // Parsers
  virtual void parse_header(const std::vector<uint8_t>& data) {
    throw std::runtime_error("parse_header() function is not defined in AbstractExtractor.");
  };

  uint16_t m_type_code;
  uint16_t m_packet_code;
  uint16_t m_controller_id;
  uint16_t m_connection_handle;
  uint16_t m_data_length;
  size_t m_header_length;

  std::vector<uint8_t>::iterator m_data_iterator;
  std::vector<uint8_t> m_data;

};

template<typename T, size_t N>
std::array<T, N> AbstractExtractor::get_array() {
  std::array<T, N> result;

  for (auto& value : result) {
    value = get_value<T>();
  }

  return result;
};

#endif //BABLE_LINUX_EXTRACTOR_HPP
