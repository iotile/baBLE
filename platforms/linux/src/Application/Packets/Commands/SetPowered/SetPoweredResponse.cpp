#include "SetPoweredResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetPoweredResponse::SetPoweredResponse(Packet::Type initial_type, Packet::Type final_type)
        : ResponsePacket(initial_type, final_type) {
      m_id = Packet::Id::SetPoweredResponse;
      m_state = false;
    }

    void SetPoweredResponse::unserialize(MGMTFormatExtractor& extractor) {
      ResponsePacket::unserialize(extractor);

      if (m_status == BaBLE::StatusCode::Success) {
        auto m_current_settings = extractor.get_value<uint32_t>();
        m_state = (m_current_settings & 1) > 0;
      }
    }

    vector<uint8_t> SetPoweredResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateSetPowered(builder, m_state);

      return builder.build(payload, BaBLE::Payload::SetPowered);
    }

    const std::string SetPoweredResponse::stringify() const {
      stringstream result;

      result << "<SetPoweredResponse> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
