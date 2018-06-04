#ifndef BABLE_LINUX_DEVICEREMOVED_HPP
#define BABLE_LINUX_DEVICEREMOVED_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class DeviceRemoved : public EventPacket<DeviceRemoved> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::EventCode::DeviceRemoved;

          case Packet::Type::HCI:
            throw std::invalid_argument("'DeviceRemoved' packet is not compatible with HCI protocol.");

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::DeviceRemoved);

          case Packet::Type::NONE:
            return 0;
        }
      };

      DeviceRemoved(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      std::array<uint8_t, 6> m_address{};
      uint8_t m_address_type;

    };

  }

}
#endif //BABLE_LINUX_DEVICEREMOVED_HPP
