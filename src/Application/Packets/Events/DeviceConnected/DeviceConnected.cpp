#include "DeviceConnected.hpp"

using namespace std;

namespace Packet::Events {

  DeviceConnected::DeviceConnected(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {
    m_address_type = 0;
    m_eir_data_length = 0;
  }

  void DeviceConnected::unserialize(MGMTFormatExtractor& extractor) {
    EventPacket::unserialize(extractor);
    m_address = extractor.get_array<uint8_t, 6>();
    m_address_type = extractor.get_value<uint8_t>();
    m_flags = extractor.get_array<uint8_t, 4>();
    m_eir_data_length = extractor.get_value<uint16_t>();

    if (m_eir_data_length > 0) {
      m_eir_data = extractor.get_vector<uint8_t>(m_eir_data_length);

      try {
        m_eir = extractor.parse_eir(m_eir_data);
      } catch (invalid_argument& err) {
        LOG.error("Can't parse EIR", "DeviceFound");
      }
    }
  };

  vector<uint8_t> DeviceConnected::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    builder
        .set_name("DeviceConnected")
        .add("Address", AsciiFormat::format_bd_address(m_address))
        .add("Address type", m_address_type)
        .add("Flags", m_flags)
        .add("EIR data length: ", m_eir_data_length)
        .add("EIR flags: ", m_eir.flags)
        .add("EIR UUID: ", AsciiFormat::format_uuid(m_eir.uuid))
        .add("EIR company ID: ", m_eir.company_id)
        .add("EIR device name", AsciiFormat::bytes_to_string(m_eir.device_name));

    return builder.build();
  };

  vector<uint8_t> DeviceConnected::serialize(FlatbuffersFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    vector<uint8_t> flags_vector(m_flags.begin(), m_flags.end());

    auto address = builder.CreateString(AsciiFormat::format_bd_address(m_address));
    auto flags = builder.CreateVector(flags_vector);
    auto uuid = builder.CreateString(AsciiFormat::format_uuid(m_eir.uuid));
    auto device_name = builder.CreateString(AsciiFormat::bytes_to_string(m_eir.device_name));

    auto payload = Schemas::CreateDeviceConnected(
        builder,
        address,
        m_address_type,
        flags,
        uuid,
        m_eir.company_id,
        device_name
    );

    return builder.build(payload, Schemas::Payload::DeviceConnected);
  }

}