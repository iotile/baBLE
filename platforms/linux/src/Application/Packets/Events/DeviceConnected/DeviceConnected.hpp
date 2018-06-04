#ifndef BABLE_LINUX_DEVICECONNECTED_HPP
#define BABLE_LINUX_DEVICECONNECTED_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class DeviceConnected : public EventPacket<DeviceConnected> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'DeviceConnected' packet not implemented with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::SubEventCode::LEConnectionComplete;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::DeviceConnected);

          case Packet::Type::NONE:
            return 0;
        }
      };

      DeviceConnected(Packet::Type initial_type, Packet::Type final_type);

      void unserialize(HCIFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

      inline const std::array<uint8_t, 6>& get_raw_device_address() const {
        return m_raw_address;
      };

      inline uint8_t get_device_address_type() const {
        return m_address_type;
      };

    private:
      uint8_t m_address_type;
      std::array<uint8_t, 6> m_raw_address{};

      std::string m_address;
    };

  }

}

#endif //BABLE_LINUX_DEVICECONNECTED_HPP
