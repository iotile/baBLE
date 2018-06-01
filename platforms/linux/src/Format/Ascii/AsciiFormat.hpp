#ifndef BABLE_LINUX_ASCIIFORMAT_HPP
#define BABLE_LINUX_ASCIIFORMAT_HPP

#include "constants.hpp"
#include "../AbstractFormat.hpp"
#include "AsciiFormatBuilder.hpp"
#include "AsciiFormatExtractor.hpp"
#include "../../utils/stream_formats.hpp"

class AsciiFormat : public AbstractFormat {

public:
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
  static T string_to_number(const std::string& str) {
    try {
      auto result = static_cast<T>(stoi(str));

    } catch (const std::bad_cast& err) {
      throw Exceptions::WrongFormatException("Can't cast given string: \"" + str + "\"");
    } catch (const std::invalid_argument& err) {
      throw Exceptions::WrongFormatException("Given string is not a number: \"" + str + "\"");
    }
  };

  const Packet::Type get_packet_type() const override {
    return Packet::Type::ASCII;
  };

  const size_t get_header_length(uint16_t type_code) const override {
    return 0;
  };

  bool is_command(uint16_t type_code) override {
    return true;
  };

  bool is_event(uint16_t type_code) override {
    return false;
  };

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    return AsciiFormatExtractor::extract_payload_length(data);
  };

  std::shared_ptr<AbstractExtractor> create_extractor(const std::vector<uint8_t>& data) override {
    return std::make_shared<AsciiFormatExtractor>(data);
  };

};

#endif //BABLE_LINUX_ASCIIFORMAT_HPP
