#ifndef BABLE_LINUX_HCIFORMAT_HPP
#define BABLE_LINUX_HCIFORMAT_HPP

#include "./constants.hpp"
#include "../AbstractFormat.hpp"
#include "HCIFormatBuilder.hpp"
#include "HCIFormatExtractor.hpp"

class HCIFormat : public AbstractFormat {

public:
  const Packet::Type packet_type() const override {
    return Packet::Type::HCI;
  };

  bool is_command(uint16_t type_code) override {
    return type_code == Format::HCI::Type::Command || type_code == Format::HCI::Type::AsyncData;
  };

  bool is_event(uint16_t type_code) override {
    return type_code == Format::HCI::Type::Event;
  };

  uint16_t extract_packet_code(const std::vector<uint8_t>& data) override {
    return HCIFormatExtractor::extract_packet_code(data);
  };

  uint16_t extract_type_code(const std::vector<uint8_t>& data) override {
    return HCIFormatExtractor::extract_type_code(data);
  };

  uint16_t extract_controller_id(const std::vector<uint8_t>& data) override {
    return HCIFormatExtractor::extract_controller_id(data);
  };

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    return HCIFormatExtractor::extract_payload_length(data);
  };

  std::shared_ptr<AbstractExtractor> create_extractor(const std::vector<uint8_t>& data) override {
    return std::make_shared<HCIFormatExtractor>(data);
  };

};

#endif //BABLE_LINUX_HCIFORMAT_HPP
