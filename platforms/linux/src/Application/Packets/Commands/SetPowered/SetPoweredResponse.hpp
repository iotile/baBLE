#ifndef BABLE_LINUX_SETPOWEREDRESPONSE_HPP
#define BABLE_LINUX_SETPOWEREDRESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet::Commands {

  class SetPoweredResponse : public ResponsePacket<SetPoweredResponse> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::SetPowered;

        case Packet::Type::HCI:
          throw std::invalid_argument("'SetPowered' packet is not compatible with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::SetPowered;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::SetPowered);

        case Packet::Type::NONE:
          return 0;
      }
    };

    SetPoweredResponse(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

  private:
    bool m_state;

  };

}

#endif //BABLE_LINUX_SETPOWEREDRESPONSE_HPP
