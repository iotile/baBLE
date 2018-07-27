#ifndef BABLE_SETSCANRESPONSE_HPP
#define BABLE_SETSCANRESPONSE_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetScanResponse : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        throw std::runtime_error("'SetScanResponseRequest' has no initial packet code (can't be registered).");
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::LESetScanResponse;
      };

      SetScanResponse();

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void set_scan_response_data(const std::vector<uint8_t>& scan_response);

    private:
      uint8_t m_scan_response_length;
      std::vector<uint8_t> m_scan_response;

    };

  }

}

#endif //BABLE_SETSCANRESPONSE_HPP
