#ifndef BABLE_LINUX_SETSCANENABLE_HPP
#define BABLE_LINUX_SETSCANENABLE_HPP

#include "../../Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetScanEnable : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        throw std::runtime_error("'SetScanEnable' has no initial packet code (can't be registered).");
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::SetScanEnable;
      };

      explicit SetScanEnable(bool state = false, bool filter_duplicates = true);

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      std::shared_ptr<Packet::AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                   const std::shared_ptr<AbstractPacket>& packet) override;

    private:
      bool m_state;
      bool m_filter_duplicates;
    };

  }

}

#endif //BABLE_LINUX_SETSCANENABLE_HPP
