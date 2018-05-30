#include "SetDiscoverableResponse.hpp"

using namespace std;

namespace Packet::Commands {

  SetDiscoverableResponse::SetDiscoverableResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::SetDiscoverableResponse;
    m_state = 0;
  }

  void SetDiscoverableResponse::unserialize(MGMTFormatExtractor& extractor) {
    ResponsePacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success){
      auto m_current_settings = extractor.get_value<uint32_t>();
      m_state = static_cast<uint8_t>((m_current_settings & 1 << 3) > 0);
    }
  }

  vector<uint8_t> SetDiscoverableResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("SetDiscoverable")
        .add("State", m_state);

    return builder.build();
  }

  vector<uint8_t> SetDiscoverableResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto payload = BaBLE::CreateSetDiscoverable(builder, m_state);

    return builder.build(payload, BaBLE::Payload::SetDiscoverable);
  }

}
