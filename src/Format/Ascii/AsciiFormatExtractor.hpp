#ifndef BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
#define BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include "./constants.hpp"
#include "../../Exceptions/InvalidCommand/InvalidCommandException.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

class AsciiFormatExtractor {

public:
  static uint16_t extract_command_code(const std::vector<uint8_t>& data);
  static uint16_t extract_controller_id(const std::vector<uint8_t>& data);

  // Constructors
  explicit AsciiFormatExtractor(const std::vector<uint8_t>& data);

  // Parsers
  void parse_payload(const std::vector<uint8_t>& data);

  // Getters
  std::string get();

  inline uint16_t get_command_code() {
    return m_command_code;
  };

  inline uint16_t get_controller_id() {
    return m_controller_id;
  };

  inline std::string get_uuid_request() {
    return m_uuid_request;
  };


private:
  std::vector<std::string> m_payload;
  size_t m_stack_pointer;

  uint16_t m_command_code;
  uint16_t m_controller_id;
  std::string m_uuid_request;

};

#endif //BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
