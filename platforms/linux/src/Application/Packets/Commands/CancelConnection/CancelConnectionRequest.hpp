#ifndef BABLE_LINUX_CANCELCONNECTION_HPP
#define BABLE_LINUX_CANCELCONNECTION_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class CancelConnectionRequest : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::CancelConnection);
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::LECreateConnectionCancel;
      };

      CancelConnectionRequest(); // Random address type

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      std::shared_ptr<AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                   const std::shared_ptr<AbstractPacket>& packet) override;

    };

  }

}

#endif //BABLE_LINUX_CANCELCONNECTION_HPP
