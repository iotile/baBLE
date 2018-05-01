#ifndef BABLE_LINUX_MGMTFORMAT_HPP
#define BABLE_LINUX_MGMTFORMAT_HPP

#include <memory>
#include "../AbstractFormat.hpp"
#include "../../Packet/AbstractPacket.hpp"
#include "MGMTFormatBuilder.hpp"
#include "MGMTFormatExtractor.hpp"

class MGMTFormat : public AbstractFormat {

public:
  const Packet::Type packet_type() const override {
    return Packet::Type::MGMT;
  };

  const size_t header_length() const override {
    return 6;
  };

  bool is_command(uint16_t type_code) override {
    return type_code == 0x0001;
  };

  bool is_event(uint16_t type_code) override {
    return 0x0002 < type_code && type_code <= 0x0025;
  };

  bool is_error(uint16_t type_code) {
    return type_code == 0x0002;
  }

  uint16_t extract_command_code(const std::vector<uint8_t>& data) override {
    return MGMTFormatExtractor::extract_command_code(data);
  }

  uint16_t extract_type_code(const std::vector<uint8_t>& data) override {
    return MGMTFormatExtractor::extract_event_code(data);
  }

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    return MGMTFormatExtractor::extract_payload_length(data);
  };

};

#endif //BABLE_LINUX_MGMTFORMAT_HPP
