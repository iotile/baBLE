#ifndef BABLE_LINUX_SETCONNECTABLERESPONSE_HPP
#define BABLE_LINUX_SETCONNECTABLERESPONSE_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetConnectableResponse : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return Format::MGMT::CommandCode::SetConnectable;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::SetConnectable);
      };

      SetConnectableResponse();

      void unserialize(MGMTFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      bool m_state;

    };

  }

}

#endif //BABLE_LINUX_SETCONNECTABLERESPONSE_HPP
