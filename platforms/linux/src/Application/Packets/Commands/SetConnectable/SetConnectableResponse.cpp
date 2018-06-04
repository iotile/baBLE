#include "SetConnectableResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetConnectableResponse::SetConnectableResponse(Packet::Type initial_type, Packet::Type final_type)
        : ResponsePacket(initial_type, final_type) {
      m_id = Packet::Id::SetConnectableResponse;
      m_state = false;
    }

    void SetConnectableResponse::unserialize(MGMTFormatExtractor& extractor) {
      ResponsePacket::unserialize(extractor);

      if (m_status == BaBLE::StatusCode::Success) {
        auto m_current_settings = extractor.get_value<uint32_t>();
        m_state = (m_current_settings & 1 << 1) > 0;
      }
    }

    vector<uint8_t> SetConnectableResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateSetConnectable(builder, m_state);

      return builder.build(payload, BaBLE::Payload::SetConnectable);
    }

    const std::string SetConnectableResponse::stringify() const {
      stringstream result;

      result << "<SetConnectableResponse> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
