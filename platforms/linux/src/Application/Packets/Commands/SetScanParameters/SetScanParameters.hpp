#ifndef BABLE_LINUX_SETSCANPARAMETERS_HPP
#define BABLE_LINUX_SETSCANPARAMETERS_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetScanParameters : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        throw std::runtime_error("'SetScanParameters' has no initial packet code (can't be registered).");
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::SetScanParameters;
      };

      explicit SetScanParameters(bool active_scan = true,
                                 uint16_t scan_interval = 0x0012, // 11.25msec
                                 uint16_t scan_window = 0x0012, // 11.25 msec
                                 uint8_t address_type = 0x01, // Random
                                 uint8_t policy = 0x00); // Accept all advertisements. Ignore directed advertisements not addressed to this device.

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      std::shared_ptr<AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                   const std::shared_ptr<AbstractPacket>& packet) override;

      void set_scan_type(bool active_scan);

    private:
      bool m_active_scan;
      uint16_t m_scan_interval;
      uint16_t m_scan_window;
      uint8_t m_address_type;
      uint8_t m_policy;
    };

  }

}

#endif //BABLE_LINUX_SETSCANPARAMETERS_HPP
