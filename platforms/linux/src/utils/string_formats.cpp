#include "string_formats.hpp"

using namespace std;

namespace Utils {

  string format_bd_address(const array<uint8_t, 6>& bd_address_array) {
    stringstream bd_address;

    for(auto it = bd_address_array.rbegin(); it != bd_address_array.rend(); ++it) {
      bd_address << RAW_HEX(*it, 2);
      if (next(it) != bd_address_array.rend()) {
        bd_address << ":";
      }
    }

    return bd_address.str();
  }

  string format_uuid(const vector<uint8_t>& uuid_vector) {
    stringstream uuid;

    for(auto& v : uuid_vector) {
      uuid << RAW_HEX(v, 2);
    }

    return uuid.str();
  }


  uint16_t uuid_to_number(const vector<uint8_t>& uuid_vector) {
    if (uuid_vector.size() != 2) {
      throw invalid_argument("Given UUID is not a 2-bytes UUID. Can't convert it to number.");
    }

    uint16_t result;

    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      result = uuid_vector.at(1) << 8 | uuid_vector.at(0);
    } else {
      result = uuid_vector.at(0) << 8 | uuid_vector.at(1);
    }

    return result;
  }

  array<uint8_t, 6> extract_bd_address(const string& bd_address) {
    array<uint8_t, 6> bd_address_array{};

    if (bd_address.empty()) {
      return bd_address_array;
    }

    string item;
    istringstream address_stream(bd_address);
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
  
  vector<uint8_t> extract_uuid(const string& uuid) {
    size_t uuid_size = uuid.size();
    if (uuid_size % 2 != 0) {  // HEX strings must have an even number of characters to be converted to bytes
      throw Exceptions::BaBLEException(
          BaBLE::StatusCode::WrongFormat,
          "Can't parse UUID: \"" + uuid + "\""
      );
    }

    vector<uint8_t> uuid_vector;
    uuid_vector.reserve(uuid_size / 2);

    for (size_t i = 0; i < uuid_size; i += 2) {
      string byte_str = uuid.substr(i, 2);
      auto byte = string_to_number<uint8_t>(byte_str, 16);
      uuid_vector.push_back(byte);
    }

    return uuid_vector;
  }

  std::vector<uint8_t> string_to_bytes(const std::string& str_data) {
    vector<uint8_t> str_bytes(str_data.begin(), str_data.end());

    return str_bytes;
  }

}