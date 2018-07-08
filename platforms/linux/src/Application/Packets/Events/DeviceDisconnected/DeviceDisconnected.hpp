#ifndef BABLE_LINUX_DEVICEDISCONNECTED_HPP
#define BABLE_LINUX_DEVICEDISCONNECTED_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Events {

    class DeviceDisconnected : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::EventCode::DisconnectComplete;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::DeviceDisconnected);
      };

      DeviceDisconnected();

      void unserialize(HCIFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      uint8_t m_status_code;
      std::string m_reason;
    };

  }

}
#endif //BABLE_LINUX_DEVICEDISCONNECTED_HPP
