#ifndef BABLE_LINUX_ABSTRACTFORMAT_HPP
#define BABLE_LINUX_ABSTRACTFORMAT_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include "../Application/Packets/constants.hpp"
#include "../Exceptions/AbstractException.hpp"
#include "AbstractExtractor.hpp"

class AbstractFormat {

public:
  virtual const Packet::Type get_packet_type() const = 0;
  virtual const size_t get_header_length(uint16_t type_code) const = 0;

  virtual uint16_t extract_payload_length(const std::vector<uint8_t>& data) = 0;

  virtual bool is_command(uint16_t type_code) = 0;
  virtual bool is_event(uint16_t type_code) = 0;

  virtual std::shared_ptr<AbstractExtractor> create_extractor(const std::vector<uint8_t>& data) = 0;

};

#endif //BABLE_LINUX_ABSTRACTFORMAT_HPP
