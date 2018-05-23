#ifndef BABLE_LINUX_SETDISCOVERABLE_HPP
#define BABLE_LINUX_SETDISCOVERABLE_HPP

#include "../CommandPacket.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

namespace Packet::Commands {

  class SetDiscoverable : public CommandPacket<SetDiscoverable> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::SetDiscoverable;

        case Packet::Type::HCI:
          throw std::invalid_argument("'SetDiscoverable' packet is not compatible with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::SetDiscoverable;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::SetDiscoverable);

        case Packet::Type::NONE:
          return 0;
      }
    };

    SetDiscoverable(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;
    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    uint8_t m_state;
    uint16_t m_timeout;

  };

}

#endif //BABLE_LINUX_SETDISCOVERABLE_HPP
