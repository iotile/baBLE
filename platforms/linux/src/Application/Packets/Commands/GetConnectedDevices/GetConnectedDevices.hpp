#ifndef BABLE_LINUX_GETCONNECTEDDEVICES_HPP
#define BABLE_LINUX_GETCONNECTEDDEVICES_HPP

#include "Application/Packets/Base/HostOnlyPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetConnectedDevices : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::GetConnectedDevices);
      };

      GetConnectedDevices();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      std::vector<Format::HCI::Device> m_connected_devices;

    };

  }

}

#endif //BABLE_LINUX_GETCONNECTEDDEVICES_HPP
