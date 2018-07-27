#ifndef BABLE_ADVERSTISINGREPORT_HPP
#define BABLE_ADVERSTISINGREPORT_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Events {

    class AdvertisingReport : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::SubEventCode::LEAdvertisingReport;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::DeviceFound);
      };

      AdvertisingReport();

      void unserialize(HCIFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      Format::HCI::AdvertisingReportType m_type;

      uint8_t m_address_type;
      std::array<uint8_t, 6> m_address{};

      uint16_t m_eir_data_length;
      std::vector<uint8_t> m_eir_data;
      Format::HCI::EIR m_eir{};

      int8_t m_rssi;
    };

  }

}

#endif //BABLE_ADVERSTISINGREPORT_HPP
