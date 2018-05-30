#include "DeviceRemoved.hpp"

using namespace std;

namespace Packet::Events {

  DeviceRemoved::DeviceRemoved(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {
    m_id = Packet::Id::DeviceRemoved;
    m_address_type = 0;
  }

  void DeviceRemoved::unserialize(MGMTFormatExtractor& extractor) {
    m_address = extractor.get_array<uint8_t, 6>();
    m_address_type = extractor.get_value<uint8_t>();
  };

  vector<uint8_t> DeviceRemoved::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    builder
        .set_name("DeviceRemoved")
        .add("Address", AsciiFormat::format_bd_address(m_address))
        .add("Address type", m_address_type);

    return builder.build();
  };

  vector<uint8_t> DeviceRemoved::serialize(FlatbuffersFormatBuilder& builder) const {
    auto address = builder.CreateString(AsciiFormat::format_bd_address(m_address));

    auto payload = BaBLE::CreateDeviceRemoved(builder, address, m_address_type);

    return builder.build(payload, BaBLE::Payload::DeviceRemoved);
  }

}