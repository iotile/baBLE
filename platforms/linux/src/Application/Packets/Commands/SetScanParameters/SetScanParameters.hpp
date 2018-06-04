#ifndef BABLE_LINUX_SETSCANPARAMETERS_HPP
#define BABLE_LINUX_SETSCANPARAMETERS_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetScanParameters : public RequestPacket<SetScanParameters> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'SetScanParameters' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::CommandCode::SetScanParameters;

          case Packet::Type::FLATBUFFERS:
            throw std::invalid_argument("'SetScanParameters' packet is not compatible with Flatbuffers protocol.");

          case Packet::Type::NONE:
            return 0;
        }
      };

      SetScanParameters(Packet::Type initial_type, Packet::Type translated_type);

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      std::shared_ptr<Packet::AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                   const std::shared_ptr<AbstractPacket>& packet) override;

      void set_scan_type(bool active);

    private:
      uint8_t m_scan_type;
      uint16_t m_scan_interval;
      uint16_t m_scan_window;
      uint8_t m_address_type;
      uint8_t m_policy;
    };

  }

}

#endif //BABLE_LINUX_SETSCANPARAMETERS_HPP
