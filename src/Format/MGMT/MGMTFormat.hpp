#ifndef BABLE_LINUX_MGMTFORMAT_HPP
#define BABLE_LINUX_MGMTFORMAT_HPP

#include "./constants.hpp"
#include "../AbstractFormat.hpp"
#include "MGMTFormatBuilder.hpp"
#include "MGMTFormatExtractor.hpp"

class MGMTFormat : public AbstractFormat {

public:
  const Packet::Type packet_type() const override {
    return Packet::Type::MGMT;
  };

  bool is_command(uint16_t type_code) override {
    return type_code == Format::MGMT::EventCode::CommandComplete
        || type_code == Format::MGMT::EventCode::CommandStatus;
  };

  bool is_event(uint16_t type_code) override {
    return 0x0002 < type_code && type_code <= 0x0025;
  };

  uint16_t extract_packet_code(const std::vector<uint8_t>& data) override {
    uint16_t type_code = extract_type_code(data);
    if (is_command(type_code)) {
      return MGMTFormatExtractor::extract_packet_code(data);
    } else if (is_event(type_code)) {
      return MGMTFormatExtractor::extract_type_code(data);
    }
  };

  uint16_t extract_controller_id(const std::vector<uint8_t>& data) override {
    return MGMTFormatExtractor::extract_controller_id(data);
  };

  uint16_t extract_type_code(const std::vector<uint8_t>& data) override {
    return MGMTFormatExtractor::extract_type_code(data);
  };

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    return MGMTFormatExtractor::extract_payload_length(data);
  };

  std::shared_ptr<AbstractExtractor> create_extractor(const std::vector<uint8_t>& data) override {
    return std::make_shared<MGMTFormatExtractor>(data);
  };

};

#endif //BABLE_LINUX_MGMTFORMAT_HPP
