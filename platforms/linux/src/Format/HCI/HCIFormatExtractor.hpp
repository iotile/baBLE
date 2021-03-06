#ifndef BABLE_HCIFORMATEXTRACTOR_HPP
#define BABLE_HCIFORMATEXTRACTOR_HPP

#include "Format/AbstractExtractor.hpp"

class HCIFormatExtractor : public AbstractExtractor {

public:
  static uint8_t extract_type_code(const std::vector<uint8_t>& data);
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data);

  static const size_t get_header_length(uint16_t type_code);

  // Constructors
  explicit HCIFormatExtractor(const std::vector<uint8_t>& data);

  Format::HCI::EIR parse_eir(const std::vector<uint8_t>& data);

private:
  // Parsers
  void parse_header(const std::vector<uint8_t>& data) override;

};

#endif //BABLE_HCIFORMATEXTRACTOR_HPP