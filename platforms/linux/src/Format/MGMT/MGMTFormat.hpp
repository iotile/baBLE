#ifndef BABLE_MGMTFORMAT_HPP
#define BABLE_MGMTFORMAT_HPP

#include "./constants.hpp"
#include "Format/AbstractFormat.hpp"
#include "MGMTFormatBuilder.hpp"
#include "MGMTFormatExtractor.hpp"

class MGMTFormat : public AbstractFormat {

public:
  const Packet::Type get_packet_type() const override {
    return Packet::Type::MGMT;
  };

  const size_t get_header_length(uint16_t type_code) const override {
    return Format::MGMT::header_length;
  };

  bool is_command(uint16_t type_code) override {
    return type_code == Format::MGMT::EventCode::CommandComplete
        || type_code == Format::MGMT::EventCode::CommandStatus;
  };

  bool is_event(uint16_t type_code) override {
    return 0x0002 < type_code && type_code <= 0x0025;
  };

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    return MGMTFormatExtractor::extract_payload_length(data);
  };

  std::shared_ptr<AbstractExtractor> create_extractor(const std::vector<uint8_t>& data) override {
    return std::make_shared<MGMTFormatExtractor>(data);
  };

};

#endif //BABLE_MGMTFORMAT_HPP
