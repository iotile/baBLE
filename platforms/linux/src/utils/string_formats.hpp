#ifndef BABLE_LINUX_STRING_FORMATS_HPP
#define BABLE_LINUX_STRING_FORMATS_HPP

#include <array>
#include <sstream>
#include <vector>
#include <Log/Log.hpp>
#include "utils/stream_formats.hpp"
#include "Exceptions/BaBLEException.hpp"

namespace Utils {

  std::string format_bd_address(const std::array<uint8_t, 6>& bd_address_array);

  std::array<uint8_t, 6> extract_bd_address(const std::string& bd_address);

  std::string format_uuid(const std::vector<uint8_t>& uuid_vector);

  uint16_t uuid_to_number(const std::vector<uint8_t>& uuid_vector);

  std::vector<uint8_t> extract_uuid(const std::string& uuid);

  std::vector<uint8_t> string_to_bytes(const std::string& uuid);

  template<typename T>
  std::string format_bytes_array(const T& bytes) {
    std::stringstream stream;

    typename T::const_iterator it;
    stream << "[";
    for(it = bytes.begin(); it != bytes.end(); ++it) {
      stream << HEX(*it);
      if (std::next(it) != bytes.end()) {
        stream << ", ";
      }
    }
    stream << "]";

    return stream.str();
  }

  template<typename T>
  std::string bytes_to_string(const T& bytes) {
    if (bytes.empty()) {
      return "";
    }

    std::stringstream result;

    typename T::const_iterator it;
    for(it = bytes.begin(); it != bytes.end(); ++it) {
      if (*it == 0) {
        break;
      }
      result << char(*it);
    }

    return result.str();
  }

  template<typename T>
  T string_to_number(const std::string& str, int base = 10) {
    try {
      auto result = static_cast<T>(stoi(str, nullptr, base));
      return result;

    } catch (const std::bad_cast& err) {
      throw Exceptions::BaBLEException(
          BaBLE::StatusCode::WrongFormat,
          "Can't cast given string to number: \"" + str + "\""
      );
    } catch (const std::invalid_argument& err) {
      throw Exceptions::BaBLEException(
          BaBLE::StatusCode::WrongFormat,
          "Given string is not a number: \"" + str + "\""
      );
    }
  }

}

#endif //BABLE_LINUX_STRING_FORMATS_HPP
