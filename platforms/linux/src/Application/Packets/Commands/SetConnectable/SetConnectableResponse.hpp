#ifndef BABLE_LINUX_SETCONNECTABLERESPONSE_HPP
#define BABLE_LINUX_SETCONNECTABLERESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    class SetConnectableResponse : public ResponsePacket<SetConnectableResponse> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::SetConnectable;

          case Packet::Type::HCI:
            throw std::invalid_argument("'SetConnectable' packet is not compatible with HCI protocol.");

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::SetConnectable);

          case Packet::Type::NONE:
            return 0;
        }
      };

      SetConnectableResponse(Packet::Type initial_type, Packet::Type final_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      bool m_state;

    };

  }

}

#endif //BABLE_LINUX_SETCONNECTABLERESPONSE_HPP
