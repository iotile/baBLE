#include "AdvertisingReport.hpp"
#include "../../../../Log/Log.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    AdvertisingReport::AdvertisingReport(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::AdvertisingReport;
      m_address_type = 0;
      m_eir_data_length = 0;
      m_rssi = 0;
      m_type = Format::HCI::AdvertisingReportType::ConnectableDirected;
    }

    void AdvertisingReport::unserialize(HCIFormatExtractor& extractor) {
      auto num_reports = extractor.get_value<uint8_t>();

      try {
        m_type = static_cast<Format::HCI::AdvertisingReportType>(extractor.get_value<uint8_t>());
      } catch (const bad_cast& err) {
        throw Exceptions::WrongFormatException("Wrong event type received in AdvertisingReport.");
      }

      m_address_type = extractor.get_value<uint8_t>();
      m_address = extractor.get_array<uint8_t, 6>();

      m_eir_data_length = extractor.get_value<uint8_t>();

      if (m_eir_data_length > 0) {
        m_eir_data = extractor.get_vector<uint8_t>(m_eir_data_length);

        try {
          m_eir = extractor.parse_eir(m_eir_data);
        } catch (invalid_argument& err) {
          LOG.error("Can't parse EIR", "AdvertisingReport");
        }
      }

      m_rssi = extractor.get_value<int8_t>();
    }

    vector<uint8_t> AdvertisingReport::serialize(AsciiFormatBuilder& builder) const {
      EventPacket::serialize(builder);

      builder
          .set_name("AdvertisingReport")
          .add("Type", m_type)
          .add("Address", AsciiFormat::format_bd_address(m_address))
          .add("Address type", m_address_type)
          .add("RSSI (dB)", m_rssi)
          .add("EIR data length: ", m_eir_data_length)
          .add("EIR flags: ", m_eir.flags)
          .add("EIR UUID: ", AsciiFormat::format_uuid(m_eir.uuid))
          .add("EIR company ID: ", m_eir.company_id)
          .add("EIR manufacturer data", m_eir.manufacturer_data)
          .add("EIR device name", AsciiFormat::bytes_to_string(m_eir.device_name));

      return builder.build();
    }

    vector<uint8_t> AdvertisingReport::serialize(FlatbuffersFormatBuilder& builder) const {
      auto address = builder.CreateString(AsciiFormat::format_bd_address(m_address));
      auto uuid = builder.CreateString(AsciiFormat::format_uuid(m_eir.uuid));
      auto device_name = builder.CreateString(AsciiFormat::bytes_to_string(m_eir.device_name));
      auto manufacturer_data = builder.CreateVector(m_eir.manufacturer_data);

      auto payload = BaBLE::CreateDeviceFound(
          builder,
          m_type,
          address,
          m_address_type,
          m_rssi,
          uuid,
          m_eir.company_id,
          manufacturer_data,
          device_name
      );

      return builder.build(payload, BaBLE::Payload::DeviceFound);
    }

  }

}
