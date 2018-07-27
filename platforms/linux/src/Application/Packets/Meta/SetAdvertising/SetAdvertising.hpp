#ifndef BABLE_SETADVERTISING_HPP
#define BABLE_SETADVERTISING_HPP

#include "Application/Packets/Base/HostOnlyPacket.hpp"
#include "Application/Packets/Commands/SetAdvertisingParameters/SetAdvertisingParameters.hpp"
#include "Application/Packets/Commands/SetAdvertiseEnable/SetAdvertiseEnable.hpp"
#include "Application/Packets/Commands/SetAdvertisingData/SetAdvertisingData.hpp"
#include "Application/Packets/Commands/SetScanResponse/SetScanResponse.hpp"

namespace Packet {

  namespace Meta {

    class SetAdvertising : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::SetAdvertising);
      };

      SetAdvertising();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void prepare(const std::shared_ptr<PacketRouter>& router) override;
      std::shared_ptr<AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<AbstractPacket>& packet);

    private:
      std::vector<uint8_t> build_advertising_data();
      std::vector<uint8_t> build_scan_response();
      Packet::Id next_step();

      Packet::Id m_waiting_response;

      std::shared_ptr<Commands::SetAdvertisingParameters> m_set_advertising_params_packet;
      std::shared_ptr<Commands::SetAdvertisingData> m_set_advertising_data_packet;
      std::shared_ptr<Commands::SetScanResponse> m_set_scan_response_packet;
      std::shared_ptr<Commands::SetAdvertiseEnable> m_set_advertise_enable_packet;

      bool m_state;
      std::vector<std::vector<uint8_t>> m_uuids_16_bits;
      std::vector<std::vector<uint8_t>> m_uuids_128_bits;
      uint16_t m_company_id;
      std::string m_name;
      std::vector<uint8_t> m_adv_manufacturer_data;
      std::vector<uint8_t> m_scan_manufacturer_data;

    };

  }

}

#endif //BABLE_SETADVERTISING_HPP
