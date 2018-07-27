#ifndef BABLE_SETADVERTISINGPARAMETERS_HPP
#define BABLE_SETADVERTISINGPARAMETERS_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetAdvertisingParameters : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        throw std::runtime_error("'SetAdvertisingParameters' has no initial packet code (can't be registered).");
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::LESetAdvertisingParameters;
      };

      explicit SetAdvertisingParameters(uint16_t interval_min = 0x0800,
                                        uint16_t interval_max = 0x0800,
                                        uint8_t advertising_type = 0x00,
                                        uint8_t own_address_type = 0x00,
                                        uint8_t direct_address_type = 0x00,
                                        uint8_t channels = 1 << 2 | 1 << 1 | 1 << 0,
                                        uint8_t policy = 0x00);

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      uint16_t m_interval_min;
      uint16_t m_interval_max;
      uint8_t m_advertising_type;
      uint8_t m_own_address_type;
      uint8_t m_direct_address_type;
      std::array<uint8_t, 6> m_address{};
      uint8_t m_channels;
      uint8_t m_policy;

    };

  }

}

#endif //BABLE_SETADVERTISINGPARAMETERS_HPP
