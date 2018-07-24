#ifndef BABLE_LINUX_DEVICECONNECTED_HPP
#define BABLE_LINUX_DEVICECONNECTED_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Events {

    class DeviceConnected : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::SubEventCode::LEConnectionComplete;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::DeviceConnected);
      };

      DeviceConnected();

      void unserialize(HCIFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      void set_socket(AbstractSocket* socket) override;

      const std::string stringify() const override;

    private:
      uint8_t m_address_type;
      std::array<uint8_t, 6> m_raw_address{};

      std::string m_address;
    };

  }

}

#endif //BABLE_LINUX_DEVICECONNECTED_HPP
