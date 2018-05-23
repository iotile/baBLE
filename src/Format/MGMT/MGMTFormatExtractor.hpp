#ifndef BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP
#define BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP

#include <array>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <vector>
#include "./constants.hpp"
#include "../../Log/Log.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"
#include "../AbstractExtractor.hpp"

class MGMTFormatExtractor : public AbstractExtractor {

public:
  static uint16_t extract_payload_length(const std::vector<uint8_t>& data);

  // Constructors
  explicit MGMTFormatExtractor(const std::vector<uint8_t>& data);

  // Parsers
  void parse_header(const std::vector<uint8_t>& data) override;
  Format::MGMT::EIR parse_eir(const std::vector<uint8_t>& data);

};

#endif //BABLE_LINUX_MGMTFORMATEXTRACTOR_HPP
