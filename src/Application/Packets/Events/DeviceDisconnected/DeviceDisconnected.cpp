#include "DeviceDisconnected.hpp"

using namespace std;

namespace Packet::Events {

  DeviceDisconnected::DeviceDisconnected(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {
    m_id = BaBLE::Payload::DeviceDisconnected;
    m_address_type = 0;
    m_raw_reason = 0;
    m_connection_handle = 0;
  }

  void DeviceDisconnected::unserialize(MGMTFormatExtractor& extractor) {
    EventPacket::unserialize(extractor);
    m_address = extractor.get_array<uint8_t, 6>();
    m_address_type = extractor.get_value<uint8_t>();
    m_raw_reason = extractor.get_value<uint8_t>();

    m_reason = Format::MGMT::Reasons[m_raw_reason];
  }

  void DeviceDisconnected::unserialize(HCIFormatExtractor& extractor) {
    EventPacket::unserialize(extractor);
    set_status(extractor.get_value<uint8_t>());
    m_connection_handle = extractor.get_value<uint16_t>();
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

  vector<uint8_t> DeviceDisconnected::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    builder
        .set_name("DeviceDisconnected")
        .add("Connection handle", m_connection_handle)
        .add("Address", AsciiFormat::format_bd_address(m_address))
        .add("Address type", m_address_type)
        .add("Reason", m_reason);

    return builder.build();
  }

  vector<uint8_t> DeviceDisconnected::serialize(FlatbuffersFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    auto address = builder.CreateString(AsciiFormat::format_bd_address(m_address));
    auto reason = builder.CreateString(m_reason);

    auto payload = BaBLE::CreateDeviceDisconnected(
        builder,
        m_connection_handle,
        address,
        m_address_type,
        reason
    );

    return builder.build(payload, BaBLE::Payload::DeviceDisconnected);
  }

}