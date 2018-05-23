#ifndef BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
#define BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include "./constants.hpp"
#include "../AbstractExtractor.hpp"
#include "../../Exceptions/InvalidCommand/InvalidCommandException.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

class AsciiFormatExtractor : public AbstractExtractor {

public:
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data);

  // Constructors
  explicit AsciiFormatExtractor(const std::vector<uint8_t>& data);

  // Parsers
  void parse_header(const std::vector<uint8_t>& data) override;

  // Getters
  std::string get_string();

  inline std::string get_uuid_request() const {
    return m_uuid_request;
  };

private:
  std::string m_uuid_request;

};

#endif //BABLE_LINUX_ASCIIFORMATEXTRACTOR_HPP
