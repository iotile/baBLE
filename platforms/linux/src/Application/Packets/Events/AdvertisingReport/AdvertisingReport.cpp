#include "AdvertisingReport.hpp"
#include "Log/Log.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    AdvertisingReport::AdvertisingReport()
        : ControllerToHostPacket(Packet::Id::AdvertisingReport, initial_type(), initial_packet_code(), final_packet_code()) {
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
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::WrongFormat,
            "Wrong event type received in AdvertisingReport."
        );
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

    vector<uint8_t> AdvertisingReport::serialize(FlatbuffersFormatBuilder& builder) const {
      auto address = builder.CreateString(Utils::format_bd_address(m_address));
      auto uuid = builder.CreateString(Utils::format_uuid(m_eir.uuid));
      auto device_name = builder.CreateString(Utils::bytes_to_string(m_eir.device_name));
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

    const string AdvertisingReport::stringify() const {
      stringstream result;

      result << "<AdvertisingReport> "
             << AbstractPacket::stringify() << ", "
             << "Type: " << to_string(m_type) << ", "
             << "Address: " << Utils::format_bd_address(m_address) << ", "
             << "Address type: " << to_string(m_address_type) << ", "
             << "RSSI (dB): " << to_string(m_rssi) << ", "
             << "EIR data length: " << to_string(m_eir_data_length) << ", "
             << "EIR flags: " << to_string(m_eir.flags) << ", "
             << "EIR UUID: " << Utils::format_uuid(m_eir.uuid) << ", "
             << "EIR company ID: " << to_string(m_eir.company_id) << ", "
             << "EIR manufacturer data: " << Utils::format_bytes_array(m_eir.manufacturer_data) << ", "
             << "EIR device name: " << Utils::bytes_to_string(m_eir.device_name);

      return result.str();
    }

  }

}
