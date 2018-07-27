#ifndef BABLE_SETCONNECTABLEREQUEST_HPP
#define BABLE_SETCONNECTABLEREQUEST_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetConnectableRequest : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::SetConnectable);
      };

      static const uint16_t final_packet_code() {
        return Format::MGMT::CommandCode::SetConnectable;
      };

      explicit SetConnectableRequest(bool state = false);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      bool m_state;

    };

  }

}

#endif //BABLE_SETCONNECTABLEREQUEST_HPP
