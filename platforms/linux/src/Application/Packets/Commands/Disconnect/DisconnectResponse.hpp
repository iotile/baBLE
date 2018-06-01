#ifndef BABLE_LINUX_DISCONNECTRESPONSE_HPP
#define BABLE_LINUX_DISCONNECTRESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    class DisconnectResponse : public ResponsePacket<DisconnectResponse> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::Disconnect;

          case Packet::Type::HCI:
            throw std::invalid_argument("'Disconnect' packet not implemented with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::CommandCode::Disconnect;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::Disconnect);

          case Packet::Type::NONE:
            return 0;
        }
      };

      DisconnectResponse(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    private:
      uint8_t m_address_type;
      std::array<uint8_t, 6> m_raw_address{};
      std::string m_address;

    };

  }

}

#endif //BABLE_LINUX_DISCONNECTRESPONSE_HPP
