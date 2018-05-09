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

  const size_t header_length() const override {
    return 0;
  };

  bool is_command(uint16_t type_code) override {
    return true;
  };

  bool is_event(uint16_t type_code) override {
    return false;
  };

  uint16_t extract_command_code(const std::vector<uint8_t>& data) override {
    flatbuffers::Verifier packet_verifier(data.data(), data.size());
    bool packet_valid = Schemas::VerifyPacketBuffer(packet_verifier);

    if (!packet_valid) {
      throw Exceptions::WrongFormatException("Flatbuffers packet is not valid. Can't extract command code from it.");
    }

    auto fb_packet = Schemas::GetPacket(data.data());
    auto payload_type = fb_packet->payload_type();

    return static_cast<uint16_t>(payload_type);
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
