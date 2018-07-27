#ifndef BABLE_SETADVERTISEENABLE_HPP
#define BABLE_SETADVERTISEENABLE_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetAdvertiseEnable : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        throw std::runtime_error("'SetAdvertiseEnable' has no initial packet code (can't be registered).");
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::LESetAdvertiseEnable;
      };

      explicit SetAdvertiseEnable(bool state = false);

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void set_state(bool state);

    private:
      bool m_state;

    };

  }

}

#endif //BABLE_SETADVERTISEENABLE_HPP
