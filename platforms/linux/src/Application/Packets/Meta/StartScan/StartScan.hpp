#ifndef BABLE_LINUX_STARTSCAN_HPP
#define BABLE_LINUX_STARTSCAN_HPP

#include "../../../AbstractPacket.hpp"
#include "../../Commands/SetScanParameters/SetScanParameters.hpp"
#include "../../Commands/SetScanEnable/SetScanEnable.hpp"

namespace Packet {

  namespace Meta {

    class StartScan : public AbstractPacket {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'StartScan' packet is a meta packet, can't be a MGMT packet.");

          case Packet::Type::HCI:
            throw std::invalid_argument("'StartScan' packet is a meta packet, can't be a HCI packet.");

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::StartScan);

          case Packet::Type::NONE:
            return 0;
        }
      };

      StartScan(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void before_sent(const std::shared_ptr<PacketRouter>& router) override;
      std::shared_ptr<AbstractPacket> on_set_scan_params_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                           const std::shared_ptr<AbstractPacket>& packet);
      std::shared_ptr<AbstractPacket> on_set_scan_enable_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                           const std::shared_ptr<AbstractPacket>& packet);

    private:
      bool m_active_scan;

      enum SubPacket {
        SetScanParameters,
        SetScanEnable,
        None
      };

      SubPacket m_waiting_response;
      uint16_t m_current_index;

      std::shared_ptr<Packet::Commands::SetScanParameters> m_set_scan_params_packet;
      std::shared_ptr<Packet::Commands::SetScanEnable> m_set_scan_enable_packet;

    };

  }

}

#endif //BABLE_LINUX_STARTSCAN_HPP
