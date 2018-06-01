#include "GetConnectedDevicesResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetConnectedDevicesResponse::GetConnectedDevicesResponse(Packet::Type initial_type, Packet::Type translated_type)
        : ResponsePacket(initial_type, translated_type) {
      m_id = Packet::Id::GetConnectedDevicesResponse;
    }

    void GetConnectedDevicesResponse::unserialize(MGMTFormatExtractor& extractor) {
      ResponsePacket::unserialize(extractor);

      if (m_status == BaBLE::StatusCode::Success) {
        auto m_num_connections = extractor.get_value<uint16_t>();
        m_devices.reserve(m_num_connections);

        std::array<uint8_t, 6> current_address{};
        uint8_t current_address_type;

        for (size_t i = 0; i < m_num_connections; i++) {
          current_address = extractor.get_array<uint8_t, 6>();
          m_devices.push_back(AsciiFormat::format_bd_address(current_address));

          current_address_type = extractor.get_value<uint8_t>();
        }
      }
    }

    vector<uint8_t> GetConnectedDevicesResponse::serialize(AsciiFormatBuilder& builder) const {
      ResponsePacket::serialize(builder);
      builder
          .set_name("GetConnectedDevices")
          .add("Num. of connections", m_devices.size())
          .add("Devices", m_devices);

      return builder.build();
    }

    vector<uint8_t> GetConnectedDevicesResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      std::vector<flatbuffers::Offset<flatbuffers::String>> devices_fb_string;
      devices_fb_string.reserve(m_devices.size());
      for (auto& value : m_devices) {
        devices_fb_string.push_back(builder.CreateString(value));
      }

      auto devices = builder.CreateVector(devices_fb_string);
      auto payload = BaBLE::CreateGetConnectedDevices(builder, devices);

      return builder.build(payload, BaBLE::Payload::GetConnectedDevices);
    }

  }

}