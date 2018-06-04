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
            return Format::MGMT::EventCode::DeviceConnected;

          case Packet::Type::HCI:
            return Format::HCI::SubEventCode::LEConnectionComplete;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::DeviceConnected);

          case Packet::Type::NONE:
            return 0;
        }
      };

      DeviceConnected(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;
      void unserialize(HCIFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

      inline const std::array<uint8_t, 6>& get_raw_device_address() const {
        return m_address;
      };

      inline uint8_t get_device_address_type() const {
        return m_address_type;
      };

    private:
      std::array<uint8_t, 6> m_address{};
      uint8_t m_address_type;
      std::array<uint8_t, 4> m_flags{};
      uint16_t m_eir_data_length;
      std::vector<uint8_t> m_eir_data;
      Format::MGMT::EIR m_eir{};
    };

  }

}

#endif //BABLE_LINUX_DEVICECONNECTED_HPP
