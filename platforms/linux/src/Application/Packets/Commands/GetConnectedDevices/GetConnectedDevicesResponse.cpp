#include "GetConnectedDevicesResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetConnectedDevicesResponse::GetConnectedDevicesResponse()
        : ControllerToHostPacket(Packet::Id::GetConnectedDevicesResponse, initial_type(), initial_packet_code(), final_packet_code()) {}

    void GetConnectedDevicesResponse::unserialize(MGMTFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());

      if (m_status == BaBLE::StatusCode::Success) {
        auto m_num_connections = extractor.get_value<uint16_t>();
        m_devices.reserve(m_num_connections);

        array<uint8_t, 6> current_address{};
        uint8_t current_address_type;

        for (size_t i = 0; i < m_num_connections; i++) {
          current_address = extractor.get_array<uint8_t, 6>();
          m_devices.push_back(Utils::format_bd_address(current_address));

          current_address_type = extractor.get_value<uint8_t>();
        }
      }
    }

    vector<uint8_t> GetConnectedDevicesResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      vector<flatbuffers::Offset<flatbuffers::String>> devices_fb_string;
      devices_fb_string.reserve(m_devices.size());
      for (auto& value : m_devices) {
        devices_fb_string.push_back(builder.CreateString(value));
      }

      auto devices = builder.CreateVector(devices_fb_string);
      auto payload = BaBLE::CreateGetConnectedDevices(builder, devices);

      return builder.build(payload, BaBLE::Payload::GetConnectedDevices);
    }

    const string GetConnectedDevicesResponse::stringify() const {
      stringstream result;

      result << "<GetConnectedDevicesRequest> "
             << AbstractPacket::stringify() << ", "
             << "Number of connections: " << to_string(m_devices.size()) << ", "
             << "Devices: [";

      for (auto it = m_devices.begin(); it != m_devices.end(); ++it) {
        result << *it;
        if (next(it) != m_devices.end()) {
          result << ", ";
        }
      }

      result << "]";

      return result.str();
    }

  }

}