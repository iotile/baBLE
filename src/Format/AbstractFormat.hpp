#ifndef BABLE_LINUX_ABSTRACTFORMAT_HPP
#define BABLE_LINUX_ABSTRACTFORMAT_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include "../Packet/constants.hpp"
#include "../Exceptions/AbstractException.hpp"

class AbstractFormat {

public:
  virtual const Packet::Type packet_type() const = 0;
  virtual const size_t header_length() const = 0;

  virtual uint16_t extract_type_code(const std::vector<uint8_t>& data) = 0;
  virtual uint16_t extract_command_code(const std::vector<uint8_t>& data) = 0;
  virtual uint16_t extract_controller_id(const std::vector<uint8_t>& data) = 0;
  virtual uint16_t extract_payload_length(const std::vector<uint8_t>& data) = 0;

  virtual bool is_command(uint16_t type_code) = 0;
  virtual bool is_event(uint16_t type_code) = 0;

};

#endif //BABLE_LINUX_ABSTRACTFORMAT_HPP
