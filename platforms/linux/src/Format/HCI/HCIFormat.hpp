#ifndef BABLE_LINUX_HCIFORMAT_HPP
#define BABLE_LINUX_HCIFORMAT_HPP

#include "Format/AbstractFormat.hpp"
#include "HCIFormatBuilder.hpp"
#include "HCIFormatExtractor.hpp"

class HCIFormat : public AbstractFormat {

public:
  const Packet::Type get_packet_type() const override {
    return Packet::Type::HCI;
  };

  const size_t get_header_length(uint16_t type_code) const override {
    return HCIFormatExtractor::get_header_length(type_code);
  };

  bool is_command(uint16_t type_code) override {
    return type_code == Format::HCI::Type::Command || type_code == Format::HCI::Type::AsyncData;
  };

  bool is_event(uint16_t type_code) override {
    return type_code == Format::HCI::Type::Event;
  };

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    return HCIFormatExtractor::extract_payload_length(data);
  };

  std::shared_ptr<AbstractExtractor> create_extractor(const std::vector<uint8_t>& data) override {
    return std::make_shared<HCIFormatExtractor>(data);
  };

};

#endif //BABLE_LINUX_HCIFORMAT_HPP
