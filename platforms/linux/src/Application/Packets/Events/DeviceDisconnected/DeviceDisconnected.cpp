#include "DeviceDisconnected.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    DeviceDisconnected::DeviceDisconnected()
        : ControllerToHostPacket(Packet::Id::DeviceDisconnected, initial_type(), initial_packet_code(), final_packet_code()) {
      m_status_code = 0;
    }

    void DeviceDisconnected::unserialize(HCIFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());
      m_connection_handle = extractor.get_value<uint16_t>();
      m_status_code = extractor.get_value<uint8_t>();

      switch (m_status_code) {
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

        case Format::HCI::StatusCode::ConnectionFailedEstablished:
          m_reason = "Connection failed to be established";

        default:
          m_reason = "Unknown reason";
          break;
      }
    }

    vector<uint8_t> DeviceDisconnected::serialize(FlatbuffersFormatBuilder& builder) const {
      auto reason = builder.CreateString(m_reason);

      auto payload = BaBLE::CreateDeviceDisconnected(
          builder,
          m_connection_handle,
          reason,
          m_status_code
      );

      return builder.build(payload, BaBLE::Payload::DeviceDisconnected);
    }

    const string DeviceDisconnected::stringify() const {
      stringstream result;

      result << "<DeviceDisconnected> "
             << AbstractPacket::stringify() << ", "
             << "Reason: " << m_reason
             << "Status code: " << m_status_code;

      return result.str();
    }

  }

}