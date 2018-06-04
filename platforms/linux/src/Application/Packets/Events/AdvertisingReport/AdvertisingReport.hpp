#ifndef BABLE_LINUX_ADVERSTISINGREPORT_HPP
#define BABLE_LINUX_ADVERSTISINGREPORT_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class AdvertisingReport : public EventPacket<AdvertisingReport> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'AdvertisingReport' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::SubEventCode::LEAdvertisingReport;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::DeviceFound);

          case Packet::Type::NONE:
            return 0;
        }
      };

      AdvertisingReport(Packet::Type initial_type, Packet::Type final_type);

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

#endif //BABLE_LINUX_ADVERSTISINGREPORT_HPP
