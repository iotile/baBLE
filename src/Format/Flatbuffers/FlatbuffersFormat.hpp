#ifndef BABLE_LINUX_FLATBUFFERSFORMAT_HPP
#define BABLE_LINUX_FLATBUFFERSFORMAT_HPP

#include <iterator>
#include <vector>
#include <Packet_generated.h>
#include "../AbstractFormat.hpp"
#include "FlatbuffersFormatBuilder.hpp"
#include "FlatbuffersFormatExtractor.hpp"
#include "../../Exceptions/WrongFormat/WrongFormatException.hpp"

class FlatbuffersFormat : public AbstractFormat {

public:
  const Packet::Type get_packet_type() const override {
    return Packet::Type::FLATBUFFERS;
  };

  const size_t get_header_length(uint16_t type_code) const override {
    return 0;
  };

  bool is_command(uint16_t type_code) override {
    return true;
  };

  bool is_event(uint16_t type_code) override {
    return false;
  };

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    return FlatbuffersFormatExtractor::extract_payload_length(data);
  };

  std::shared_ptr<AbstractExtractor> create_extractor(const std::vector<uint8_t>& data) override {
    return std::make_shared<FlatbuffersFormatExtractor>(data);
  };

private:

};

#endif //BABLE_LINUX_FLATBUFFERSFORMAT_HPP
