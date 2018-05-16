#ifndef BABLE_LINUX_COMMANDS_STOPSCAN_HPP
#define BABLE_LINUX_COMMANDS_STOPSCAN_HPP

#include "../CommandPacket.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

namespace Packet::Commands {

  class StopScan : public CommandPacket<StopScan> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::StopScan;

        case Packet::Type::HCI:
          throw std::invalid_argument("'StopScan' packet is not implemented with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::StopScan;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::StopScan);

        case Packet::Type::NONE:
          return 0;
      }
    };

    StopScan(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;
    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    uint8_t m_address_type;

  };

}

#endif //BABLE_LINUX_COMMANDS_STOPSCAN_HPP
