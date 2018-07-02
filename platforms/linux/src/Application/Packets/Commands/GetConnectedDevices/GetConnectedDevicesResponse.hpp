#ifndef BABLE_LINUX_GETCONNECTEDDEVICESRESPONSE_HPP
#define BABLE_LINUX_GETCONNECTEDDEVICESRESPONSE_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetConnectedDevicesResponse : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return Format::MGMT::CommandCode::GetConnections;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::GetConnectedDevices);
      };

      GetConnectedDevicesResponse();

      void unserialize(MGMTFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      std::vector<std::string> m_devices;

    };

  }

}

#endif //BABLE_LINUX_GETCONNECTEDDEVICESRESPONSE_HPP
