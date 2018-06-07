#ifndef BABLE_LINUX_HCIFORMATEXTRACTOR_HPP
#define BABLE_LINUX_HCIFORMATEXTRACTOR_HPP

#include <array>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <vector>
#include "./constants.hpp"
#include "../AbstractExtractor.hpp"
#include "../../Log/Log.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

class HCIFormatExtractor : public AbstractExtractor {

public:
  static uint8_t extract_type_code(const std::vector<uint8_t>& data);
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data);

  static const size_t get_header_length(uint16_t type_code);

  // Constructors
  explicit HCIFormatExtractor(const std::vector<uint8_t>& data);

private:
  // Parsers
  void parse_header(const std::vector<uint8_t>& data) override;

};

#endif //BABLE_LINUX_HCIFORMATEXTRACTOR_HPP