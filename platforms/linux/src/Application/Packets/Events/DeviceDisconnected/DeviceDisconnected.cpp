#include "DeviceDisconnected.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    DeviceDisconnected::DeviceDisconnected()
        : ControllerToHostPacket(Packet::Id::DeviceDisconnected, initial_type(), initial_packet_code(), final_packet_code()) {
      m_raw_reason = 0;
    }

    void DeviceDisconnected::unserialize(HCIFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());
      m_connection_id = extractor.get_value<uint16_t>();
      m_raw_reason = extractor.get_value<uint8_t>();

      switch (m_raw_reason) {
        case Format::HCI::StatusCode::ConnectionTimeout:
          m_reason = "Connection timeout";
          break;

        case Format::HCI::StatusCode::ConnectionTerminatedLocalHost:
          m_reason = "Connection terminated by local host";
          break;

        case Format::HCI::StatusCode::RemoteUserTerminatedConnection:
        case Format::HCI::StatusCode::RemoteDeviceTerminatedConnectionLowResources:
        case Format::HCI::StatusCode::RemoteDeviceTerminatedConnectionPowerOff:
          m_reason = "Connection terminated by remote host";
          break;

        case Format::HCI::StatusCode::AuthenticationFailed:
        case Format::HCI::StatusCode::PINOrKeyMissing:
          m_reason = "Connection terminated due to authentication failure";
          break;

        default:
          m_reason = "Error code: " + to_string(m_raw_reason);
          break;
      }
    }

    vector<uint8_t> DeviceDisconnected::serialize(FlatbuffersFormatBuilder& builder) const {
      auto reason = builder.CreateString(m_reason);

      auto payload = BaBLE::CreateDeviceDisconnected(
          builder,
          m_connection_id,
          reason
      );

      return builder.build(payload, BaBLE::Payload::DeviceDisconnected);
    }

    const string DeviceDisconnected::stringify() const {
      stringstream result;

      result << "<DeviceDisconnected> "
             << AbstractPacket::stringify() << ", "
             << "Reason: " << m_reason;

      return result.str();
    }

  }

}