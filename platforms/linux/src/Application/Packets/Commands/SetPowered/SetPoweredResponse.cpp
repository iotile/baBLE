#include "SetPoweredResponse.hpp"

using namespace std;

namespace Packet::Commands {

  SetPoweredResponse::SetPoweredResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::SetPoweredResponse;
    m_state = false;
  }

  void SetPoweredResponse::unserialize(MGMTFormatExtractor& extractor) {
    ResponsePacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success){
      auto m_current_settings = extractor.get_value<uint32_t>();
      m_state = (m_current_settings & 1) > 0;
    }
  }

  vector<uint8_t> SetPoweredResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("SetPowered")
        .add("State", m_state);

    return builder.build();
  }

  vector<uint8_t> SetPoweredResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto payload = BaBLE::CreateSetPowered(builder, m_state);

    return builder.build(payload, BaBLE::Payload::SetPowered);
  }

}
