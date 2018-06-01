#ifndef BABLE_LINUX_DEVICEFOUND_HPP
#define BABLE_LINUX_DEVICEFOUND_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class DeviceFound : public EventPacket<DeviceFound> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::EventCode::DeviceFound;

          case Packet::Type::HCI:
            throw std::invalid_argument("'DeviceFound' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::EventCode::DeviceFound;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::DeviceFound);

          case Packet::Type::NONE:
            return 0;
        }
      };

      DeviceFound(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    private:
      std::array<uint8_t, 6> m_address{};
      uint8_t m_address_type;
      int8_t m_rssi;
      std::array<uint8_t, 4> m_flags{};
      uint16_t m_eir_data_length;
      std::vector<uint8_t> m_eir_data;
      Format::MGMT::EIR m_eir{};
    };

  }

}

#endif //BABLE_LINUX_DEVICEFOUND_HPP
