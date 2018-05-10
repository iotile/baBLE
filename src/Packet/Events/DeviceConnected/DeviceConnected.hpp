#ifndef BABLE_LINUX_DEVICECONNECTED_HPP
#define BABLE_LINUX_DEVICECONNECTED_HPP

#include "../EventPacket.hpp"

namespace Packet::Events {

  class DeviceConnected : public EventPacket<DeviceConnected> {

  public:
    static const uint16_t event_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::EventCode::DeviceConnected;

        case Packet::Type::ASCII:
          return Format::Ascii::EventCode::DeviceConnected;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::DeviceConnected);
      }
    };

    DeviceConnected(Packet::Type initial_type, Packet::Type translated_type);

    void import(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

  private:
    std::array<uint8_t, 6> m_address{};
    uint8_t m_address_type;
    std::array<uint8_t, 4> m_flags{};
    uint16_t m_eir_data_length;
    std::vector<uint8_t> m_eir_data;
    EIR m_eir{};
  };

}

#endif //BABLE_LINUX_DEVICECONNECTED_HPP
