#include "LEAdvertisingReport.hpp"

using namespace std;

namespace Packet::Events {

  LEAdvertisingReport::LEAdvertisingReport(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {
    m_id = BaBLE::Payload::NONE;
    m_address_type = 0;
    m_flags = 0;
    m_company_id = 0;
    m_rssi = 0;
  }

  void LEAdvertisingReport::unserialize(HCIFormatExtractor& extractor) {
    EventPacket::unserialize(extractor);

    auto num_reports = extractor.get_value<uint8_t>();
    auto event_type = extractor.get_value<uint8_t>();
    m_address_type = extractor.get_value<uint8_t>();
    m_address = extractor.get_array<uint8_t, 6>();
    auto data_length = extractor.get_value<uint8_t>();

    while (data_length > 0) {
      auto report_length = extractor.get_value<uint8_t>();
      data_length -= report_length + 1;
      auto report_type = extractor.get_value<uint8_t>();

      switch (report_type) {

        case Format::HCI::ReportType::Flags:
          m_flags = extractor.get_value<uint8_t>();
          break;

        case Format::HCI::ReportType::UUID128:
          m_uuid = extractor.get_vector<uint8_t>(static_cast<size_t>(report_length - 1));
          break;

        case Format::HCI::ReportType::ManufacturerSpecific:
        {
          m_company_id = extractor.get_value<uint16_t>();
          auto data = extractor.get_vector<uint8_t>(static_cast<size_t>(report_length - 1 - sizeof(m_company_id)));
          break;
        }

        case Format::HCI::ReportType::DeviceName:
        {
          std::vector<uint8_t> raw_device_name = extractor.get_vector<uint8_t>(static_cast<size_t>(report_length - 1));
          m_device_name = AsciiFormat::bytes_to_string(raw_device_name);
          break;
        }

        default:
          LOG.warning("Unknown HCI Advertising Report type: " + to_string(report_type));
          break;
      }
    }

    m_rssi = extractor.get_value<int8_t>();

  }

  vector<uint8_t> LEAdvertisingReport::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);
    builder
        .set_name("LEAdvertisingReport")
        .add("Address type", m_address_type)
        .add("Address", AsciiFormat::format_bd_address(m_address))
        .add("Flags", m_flags)
        .add("UUID", AsciiFormat::format_uuid(m_uuid))
        .add("Company ID", m_company_id)
        .add("Device name", m_device_name)
        .add("RSSI (dB)", m_rssi);

    return builder.build();
  }

}
