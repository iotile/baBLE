#ifndef BABLE_LINUX_SETDISCOVERABLERESPONSE_HPP
#define BABLE_LINUX_SETDISCOVERABLERESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet::Commands {

  class SetDiscoverableResponse : public ResponsePacket<SetDiscoverableResponse> {

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

    SetDiscoverableResponse(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

  private:
    uint8_t m_state;

  };

}

#endif //BABLE_LINUX_SETDISCOVERABLERESPONSE_HPP
