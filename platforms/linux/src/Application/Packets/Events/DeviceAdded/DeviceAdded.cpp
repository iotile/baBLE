#include "DeviceAdded.hpp"

using namespace std;

namespace Packet::Events {

  DeviceAdded::DeviceAdded(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {
    m_id = Packet::Id::DeviceAdded;
    m_address_type = 0;
    m_action = 0;
  }

  void DeviceAdded::unserialize(MGMTFormatExtractor& extractor) {
    m_address = extractor.get_array<uint8_t, 6>();
    m_address_type = extractor.get_value<uint8_t>();
    m_action = extractor.get_value<uint8_t>();
  };

  vector<uint8_t> DeviceAdded::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    builder
        .set_name("DeviceAdded")
        .add("Address", AsciiFormat::format_bd_address(m_address))
        .add("Address type", m_address_type)
        .add("Action", m_action);

    return builder.build();
  };

  vector<uint8_t> DeviceAdded::serialize(FlatbuffersFormatBuilder& builder) const {
    auto address = builder.CreateString(AsciiFormat::format_bd_address(m_address));

    auto payload = BaBLE::CreateDeviceAdded(builder, address, m_address_type);

    return builder.build(payload, BaBLE::Payload::DeviceAdded);
  }

}