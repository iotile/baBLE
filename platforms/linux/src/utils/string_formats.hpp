#ifndef BABLE_LINUX_STRING_FORMATS_HPP
#define BABLE_LINUX_STRING_FORMATS_HPP

#include <array>
#include <sstream>
#include <vector>
#include "./stream_formats.hpp"
#include "../Exceptions/WrongFormat/WrongFormatException.hpp"

namespace Utils {

  static std::string format_bd_address(const std::array<uint8_t, 6>& bd_address_array) {
    std::stringstream bd_address;

    for(auto it = bd_address_array.rbegin(); it != bd_address_array.rend(); ++it) {
      bd_address << RAW_HEX(*it, 2);
      if (std::next(it) != bd_address_array.rend()) {
        bd_address << ":";
      }
    }

    return bd_address.str();
  };

  static std::string format_uuid(const std::vector<uint8_t>& uuid_vector) {
    std::stringstream uuid;

    for(auto& v : uuid_vector) {
      uuid << RAW_HEX(v, 2);
    }

    return uuid.str();
  };

  template<typename T>
  static std::string format_bytes_array(const T& bytes) {
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
  };

  template<typename T>
  static std::string bytes_to_string(const T& bytes) {
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
  };

  template<typename T>
  static T string_to_number(const std::string& str, int base = 10) {
    try {
      auto result = static_cast<T>(stoi(str, nullptr, base));

    } catch (const std::bad_cast& err) {
      throw Exceptions::WrongFormatException("Can't cast given string: \"" + str + "\"");
    } catch (const std::invalid_argument& err) {
      throw Exceptions::WrongFormatException("Given string is not a number: \"" + str + "\"");
    }
  };

  static std::array<uint8_t, 6> extract_bd_address(const std::string& bd_address) {
    std::array<uint8_t, 6> bd_address_array{};

    if (bd_address.empty()) {
      return bd_address_array;
    }

    std::string item;
    std::istringstream address_stream(bd_address);
    for (auto it = bd_address_array.rbegin(); it != bd_address_array.rend(); ++it) {
      if (!getline(address_stream, item, ':')) {
        throw Exceptions::WrongFormatException("Can't parse Bluetooth device MAC address.");
      }
      *it = string_to_number<uint8_t>(item, 16);
    }

    return bd_address_array;
  };

}

#endif //BABLE_LINUX_STRING_FORMATS_HPP
