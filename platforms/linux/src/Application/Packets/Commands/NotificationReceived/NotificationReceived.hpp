#ifndef BABLE_LINUX_NOTIFICATIONRECEIVED_HPP
#define BABLE_LINUX_NOTIFICATIONRECEIVED_HPP

#include "../../Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    // Is a response because type_code == Format::HCI::AsyncData, but is considered as an event afterward
    class NotificationReceived : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::AttributeCode::HandleValueNotification;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::NotificationReceived);
      };

      NotificationReceived();

      void unserialize(HCIFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      uint16_t m_attribute_handle;
      std::vector<uint8_t> m_value;
    };

  }

}

#endif //BABLE_LINUX_NOTIFICATIONRECEIVED_HPP
