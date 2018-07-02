#ifndef BABLE_LINUX_GETCONNECTEDDEVICESREQUEST_HPP
#define BABLE_LINUX_GETCONNECTEDDEVICESREQUEST_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetConnectedDevicesRequest : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::GetConnectedDevices);
      };

      static const uint16_t final_packet_code() {
        return Format::MGMT::CommandCode::GetConnections;
      };

      GetConnectedDevicesRequest();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

      const std::string stringify() const override;
    };

  }

}

#endif //BABLE_LINUX_GETCONNECTEDDEVICESREQUEST_HPP
