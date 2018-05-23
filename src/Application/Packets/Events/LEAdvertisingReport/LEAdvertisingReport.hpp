#ifndef BABLE_LINUX_LEADVERSTISINGREPORT_HPP
#define BABLE_LINUX_LEADVERSTISINGREPORT_HPP

#include "../EventPacket.hpp"
#include "../../../../Exceptions/NotFound/NotFoundException.hpp"

namespace Packet::Events {

  class LEAdvertisingReport : public EventPacket<LEAdvertisingReport> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'LEAdvertisingReport' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::SubEventCode::LEAdvertisingReport;

        case Packet::Type::ASCII:
          return Format::Ascii::EventCode::LEAdvertisingReport;

        case Packet::Type::FLATBUFFERS:
          throw Exceptions::NotFoundException("LEAdvertisingReport event has no event code for Flatbuffers.");

        case Packet::Type::NONE:
          return 0;
      }
    };

    LEAdvertisingReport(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(HCIFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;

  private:
    uint8_t m_address_type;
    std::array<uint8_t, 6> m_address{};

    uint8_t m_flags;
    std::vector<uint8_t> m_uuid;
    uint16_t m_company_id;
    std::string m_device_name;

    int8_t m_rssi;
  };

}


#endif //BABLE_LINUX_LEADVERSTISINGREPORT_HPP
