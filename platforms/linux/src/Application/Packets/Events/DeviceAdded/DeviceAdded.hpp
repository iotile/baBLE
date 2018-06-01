#ifndef BABLE_LINUX_DEVICEADDED_HPP
#define BABLE_LINUX_DEVICEADDED_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class DeviceAdded : public EventPacket<DeviceAdded> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::EventCode::DeviceAdded;

          case Packet::Type::HCI:
            throw std::invalid_argument("'DeviceAdded' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::EventCode::DeviceAdded;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::DeviceAdded);

          case Packet::Type::NONE:
            return 0;
        }
      };

      DeviceAdded(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    private:
      std::array<uint8_t, 6> m_address{};
      uint8_t m_address_type;
      uint8_t m_action;

    };

  }

}

#endif //BABLE_LINUX_DEVICEADDED_HPP
