#include "RemoveDeviceResponse.hpp"

using namespace std;

namespace Packet::Commands {

  RemoveDeviceResponse::RemoveDeviceResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::RemoveDeviceResponse;
    m_address_type = 0;
  }

  void RemoveDeviceResponse::unserialize(MGMTFormatExtractor& extractor) {
    ResponsePacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success){
      m_address = extractor.get_array<uint8_t, 6>();
      m_address_type = extractor.get_value<uint8_t>();
    }
  }

  vector<uint8_t> RemoveDeviceResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("RemoveDevice")
        .add("Address type", m_address_type)
        .add("Address", m_address);

    return builder.build();
  }

  // TODO: send string address
  vector<uint8_t> RemoveDeviceResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto address = builder.CreateVector(m_address.data(), m_address.size());

    auto payload = BaBLE::CreateRemoveDevice(builder, address, m_address_type);

    return builder.build(payload, BaBLE::Payload::RemoveDevice);
  }

}
