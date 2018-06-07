#ifndef BABLE_LINUX_DEVICEDISCONNECTED_HPP
#define BABLE_LINUX_DEVICEDISCONNECTED_HPP

#include "../EventPacket.hpp"

namespace Packet::Events {

  class DeviceDisconnected : public EventPacket<DeviceDisconnected> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::EventCode::DeviceDisconnected;

        case Packet::Type::HCI:
          return Format::HCI::EventCode::DisconnectComplete;

        case Packet::Type::ASCII:
          return Format::Ascii::EventCode::DeviceDisconnected;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::DeviceDisconnected);

        case Packet::Type::NONE:
          return 0;
      }
    };

    DeviceDisconnected(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(MGMTFormatExtractor& extractor) override;
    void unserialize(HCIFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    inline uint16_t get_connection_handle() const {
      return m_connection_handle;
    };

  private:
    std::array<uint8_t, 6> m_address{};
    uint8_t m_address_type;
    uint8_t m_raw_reason;
    std::string m_reason;

    uint16_t m_connection_handle;
  };

}
#endif //BABLE_LINUX_DEVICEDISCONNECTED_HPP
