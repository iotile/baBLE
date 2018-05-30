#include "SetConnectableResponse.hpp"

using namespace std;

namespace Packet::Commands {

  SetConnectableResponse::SetConnectableResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::SetConnectableResponse;
    m_state = false;
  }

  void SetConnectableResponse::unserialize(MGMTFormatExtractor& extractor) {
    ResponsePacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success){
      auto m_current_settings = extractor.get_value<uint32_t>();
      m_state = (m_current_settings & 1 << 1) > 0;
    }
  }

  vector<uint8_t> SetConnectableResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("SetConnectable")
        .add("State", m_state);

    return builder.build();
  }

  vector<uint8_t> SetConnectableResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto payload = BaBLE::CreateSetConnectable(builder, m_state);

    return builder.build(payload, BaBLE::Payload::SetConnectable);
  }

}
