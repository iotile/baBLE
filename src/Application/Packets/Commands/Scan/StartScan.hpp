#ifndef BABLE_LINUX_COMMANDS_STARTSCAN_HPP
#define BABLE_LINUX_COMMANDS_STARTSCAN_HPP

#include "../CommandPacket.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

namespace Packet::Commands {

  class StartScan : public CommandPacket<StartScan> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::StartScan;

        case Packet::Type::HCI:
          throw std::invalid_argument("'StartScan' packet is not implemented with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::StartScan;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::StartScan);

        case Packet::Type::NONE:
          return 0;
      }
    };

    StartScan(Packet::Type initial_type, Packet::Type translated_type);

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

#endif //BABLE_LINUX_COMMANDS_STARTSCAN_HPP
