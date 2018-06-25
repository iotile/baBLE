#include "AbstractExtractor.hpp"
#include "../Exceptions/BaBLEException.hpp"

AbstractExtractor::AbstractExtractor(const std::vector<uint8_t>& data) {
  m_type_code = 0;
  m_packet_code = 0;
  m_controller_id = NON_CONTROLLER_ID;
  m_connection_handle = 0;
  m_header_length = 0;
  m_data.assign(data.begin(), data.end());
  m_data_length = static_cast<uint16_t>(m_data.size());
  m_data_iterator = m_data.begin();
}


template<typename T>
T AbstractExtractor::get_value() {
  const size_t nb_bytes = sizeof(T);

  if (m_data_iterator + nb_bytes > m_data.end()) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::WrongFormat,
        "Can't deserialize given type: not enough bytes in payload."
    );
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
}
template int8_t AbstractExtractor::get_value<int8_t>();
template uint8_t AbstractExtractor::get_value<uint8_t>();
template uint16_t AbstractExtractor::get_value<uint16_t>();
template uint32_t AbstractExtractor::get_value<uint32_t>();
template char AbstractExtractor::get_value<char>();

template<typename T>
std::vector<T> AbstractExtractor::get_vector(size_t length) {
  std::vector<T> result(length);

  for (auto& value : result) {
    value = get_value<T>();
  }

  return result;
}
template std::vector<uint8_t> AbstractExtractor::get_vector<uint8_t>();
template std::vector<uint16_t> AbstractExtractor::get_vector<uint16_t>();

template<typename T>
std::vector<T> AbstractExtractor::get_vector() {
  std::vector<T> result;

  while (true) {
    try {
      auto value = get_value<T>();
      result.push_back(value);
    } catch (const Exceptions::BaBLEException&) {
      break;
    }
  }

  return result;
}
template std::vector<uint8_t> AbstractExtractor::get_vector<uint8_t>(size_t);
template std::vector<uint16_t> AbstractExtractor::get_vector<uint16_t>(size_t);

void AbstractExtractor::set_data_pointer(size_t value) {
  if (value < 0 || value > m_data.size()) {
    throw std::invalid_argument("Trying to set a new data pointer value which is out of bonds.");
  }

  if (value == m_data.size()) {
    m_data_iterator = m_data.end();
  } else {
    m_data_iterator = m_data.begin() + value;
  }
}

void AbstractExtractor::set_controller_id(uint16_t controller_id) {
  m_controller_id = controller_id;
}

