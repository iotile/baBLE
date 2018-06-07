#ifndef BABLE_LINUX_NOTIFICATIONRECEIVED_HPP
#define BABLE_LINUX_NOTIFICATIONRECEIVED_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    // Is a response because type_code == Format::HCI::AsyncData, but is considered as an event afterward
    class NotificationReceived : public ResponsePacket<NotificationReceived> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'NotificationReceived' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::AttributeCode::HandleValueNotification;

          case Packet::Type::ASCII:
            return Format::Ascii::CommandCode::NotificationReceived;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::NotificationReceived);

          case Packet::Type::NONE:
            return 0;
        }
      };

      NotificationReceived(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(HCIFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    private:
      uint16_t m_attribute_handle;
      std::vector<uint8_t> m_value;
    };

  }

}

#endif //BABLE_LINUX_NOTIFICATIONRECEIVED_HPP
