#ifndef BABLE_CREATECONNECTION_HPP
#define BABLE_CREATECONNECTION_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class CreateConnection : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::Connect);
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::LECreateConnection;
      };

      explicit CreateConnection(const std::string& address = "",
                                uint8_t address_type = 0x01,  // Random address type
                                float connection_interval_min = 7.5,  // Minimum connection interval (7.5ms)
                                float connection_interval_max = 30);  // Maximum connection interval (30ms)

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;


      const std::string stringify() const override;

      void prepare(const std::shared_ptr<PacketRouter>& router) override;
      std::shared_ptr<AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                   const std::shared_ptr<AbstractPacket>& packet) override;

    private:
      uint16_t m_scan_interval;
      uint16_t m_scan_window;
      uint8_t m_policy;
      uint8_t m_peer_address_type;
      std::array<uint8_t, 6> m_raw_address{};
      uint8_t m_own_address_type;
      uint16_t m_connection_interval_min;
      uint16_t m_connection_interval_max;
      uint16_t m_connection_latency;
      uint16_t m_supervision_timeout;
      uint16_t m_min_ce_length;
      uint16_t m_max_ce_length;

      std::string m_address;
      bool m_response_received;
    };

  }

}

#endif //BABLE_CREATECONNECTION_HPP
