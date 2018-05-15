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
  const Packet::Type packet_type() const override {
    return Packet::Type::FLATBUFFERS;
  };

  bool is_command(uint16_t type_code) override {
    return true;
  };

  bool is_event(uint16_t type_code) override {
    return false;
  };

  uint16_t extract_command_code(const std::vector<uint8_t>& data) override {
    return FlatbuffersFormatExtractor::extract_payload_type(data);
  }

  uint16_t extract_controller_id(const std::vector<uint8_t>& data) override {
    return FlatbuffersFormatExtractor::extract_controller_id(data);
  }

  uint16_t extract_type_code(const std::vector<uint8_t>& data) override {
    return 0;
  }

  uint16_t extract_payload_length(const std::vector<uint8_t>& data) override {
    throw std::runtime_error("Flatbuffers format can't extract payload length: this information is not included in the format.");
  };

private:

};

#endif //BABLE_LINUX_FLATBUFFERSFORMAT_HPP
