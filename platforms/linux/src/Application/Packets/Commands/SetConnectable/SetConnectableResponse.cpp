#include <sstream>
#include "SetConnectableResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetConnectableResponse::SetConnectableResponse()
        : ControllerToHostPacket(Packet::Id::SetConnectableResponse, initial_type(), initial_packet_code(), final_packet_code()) {
      m_state = false;
    }

    void SetConnectableResponse::unserialize(MGMTFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());

      if (m_status == BaBLE::StatusCode::Success) {
        auto m_current_settings = extractor.get_value<uint32_t>();
        m_state = (m_current_settings & 1 << 1) > 0;
      }
    }

    vector<uint8_t> SetConnectableResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateSetConnectable(builder, m_state);

      return builder.build(payload, BaBLE::Payload::SetConnectable);
    }

    const string SetConnectableResponse::stringify() const {
      stringstream result;

      result << "<SetConnectableResponse> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
