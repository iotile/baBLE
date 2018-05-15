#include "DeviceDisconnected.hpp"

using namespace std;

namespace Packet::Events {

  DeviceDisconnected::DeviceDisconnected(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {
    m_address_type = 0;
    m_raw_reason = 0;
  }

  void DeviceDisconnected::unserialize(MGMTFormatExtractor& extractor) {
    EventPacket::unserialize(extractor);
    m_address = extractor.get_array<uint8_t, 6>();
    m_address_type = extractor.get_value<uint8_t>();
    m_raw_reason = extractor.get_value<uint8_t>();

    m_reason = Format::MGMT::Reasons[m_raw_reason];
  };

  vector<uint8_t> DeviceDisconnected::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    builder
        .set_name("DeviceDisconnected")
        .add("Address", AsciiFormat::format_bd_address(m_address))
        .add("Address type", m_address_type)
        .add("Reason", m_reason);

    return builder.build();
  };

  vector<uint8_t> DeviceDisconnected::serialize(FlatbuffersFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    auto address = builder.CreateString(AsciiFormat::format_bd_address(m_address));
    auto reason = builder.CreateString(m_reason);

    auto payload = Schemas::CreateDeviceDisconnected(
        builder,
        address,
        m_address_type,
        reason
    );

    return builder.build(m_controller_id, payload, Schemas::Payload::DeviceDisconnected, m_native_class);
  }

}