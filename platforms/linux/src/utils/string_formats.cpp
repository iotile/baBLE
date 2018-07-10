#include "string_formats.hpp"

namespace Utils {

  std::string format_bd_address(const std::array<uint8_t, 6>& bd_address_array) {
    std::stringstream bd_address;

    for(auto it = bd_address_array.rbegin(); it != bd_address_array.rend(); ++it) {
      bd_address << RAW_HEX(*it, 2);
      if (std::next(it) != bd_address_array.rend()) {
        bd_address << ":";
      }
    }

    return bd_address.str();
  }

  std::string format_uuid(const std::vector<uint8_t>& uuid_vector) {
    std::stringstream uuid;

    for(auto& v : uuid_vector) {
      uuid << RAW_HEX(v, 2);
    }

    return uuid.str();
  }

  std::array<uint8_t, 6> extract_bd_address(const std::string& bd_address) {
    std::array<uint8_t, 6> bd_address_array{};

    if (bd_address.empty()) {
      return bd_address_array;
    }

    std::string item;
    std::istringstream address_stream(bd_address);
    for (auto it = bd_address_array.rbegin(); it != bd_address_array.rend(); ++it) {
      if (!getline(address_stream, item, ':')) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::WrongFormat,
            "Can't parse Bluetooth device MAC address: \"" + bd_address + "\""
        );
      }
      *it = string_to_number<uint8_t>(item, 16);
    }

    return bd_address_array;
  }

}