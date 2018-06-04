#ifndef BABLE_LINUX_CREATECONNECTION_HPP
#define BABLE_LINUX_CREATECONNECTION_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class CreateConnection : public RequestPacket<CreateConnection> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'CreateConnection' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::CommandCode::LECreateConnection;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::Connect);

          case Packet::Type::NONE:
            return 0;
        }
      };

      CreateConnection(Packet::Type initial_type, Packet::Type final_type);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void before_sent(const std::shared_ptr<PacketRouter>& router) override;

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
    };

  }

}

#endif //BABLE_LINUX_CREATECONNECTION_HPP
