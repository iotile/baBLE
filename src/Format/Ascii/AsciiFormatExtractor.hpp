#ifndef BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
#define BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include "./constants.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

class AsciiFormatExtractor {

public:
  static uint16_t extract_command_code(const std::vector<uint8_t>& data);

  // Constructors
  explicit AsciiFormatExtractor(const std::vector<uint8_t>& data);

  // Parsers
  void parse_payload(const std::vector<uint8_t>& data);

  // Getters
  std::string get();


private:
  std::vector<std::string> m_payload;
  size_t m_stack_pointer;

};

#endif //BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
