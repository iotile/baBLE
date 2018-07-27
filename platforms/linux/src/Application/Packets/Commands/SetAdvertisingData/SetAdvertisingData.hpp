#ifndef BABLE_SETADVERTISINGDATA_HPP
#define BABLE_SETADVERTISINGDATA_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetAdvertisingData : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        throw std::runtime_error("'SetAdvertisingData' has no initial packet code (can't be registered).");
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::LESetAdvertisingData;
      };

      SetAdvertisingData();

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void set_advertising_data(const std::vector<uint8_t>& advertising_data);

    private:
      uint8_t m_advertising_data_length;
      std::vector<uint8_t> m_advertising_data;

    };

  }

}

#endif //BABLE_SETADVERTISINGDATA_HPP
