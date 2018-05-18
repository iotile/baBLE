#ifndef BABLE_LINUX_NOTIFICATIONRECEIVED_HPP
#define BABLE_LINUX_NOTIFICATIONRECEIVED_HPP

#include "../CommandPacket.hpp"

namespace Packet::Commands {

  // TODO: do better -> is a command because type = Data but acts like an event...
  class NotificationReceived : public CommandPacket<NotificationReceived> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'NotificationReceived' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::AttributeCode::HandleValueNotification;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::NotificationReceived;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::NotificationReceived);

        case Packet::Type::NONE:
          return 0;
      }
    };

    NotificationReceived(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override {
      throw Exceptions::InvalidCommandException("Can't send a 'NotificationReceived packet...");
    };

    void unserialize(FlatbuffersFormatExtractor& extractor) override {
      throw Exceptions::InvalidCommandException("Can't send a 'NotificationReceived packet...");
    };

    void unserialize(HCIFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

  private:
    uint16_t m_connection_handle;
    uint16_t m_attribute_handle;
    std::vector<uint8_t> m_value;
  };

}

#endif //BABLE_LINUX_NOTIFICATIONRECEIVED_HPP
