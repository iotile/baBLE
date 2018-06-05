#include <sstream>
#include "SetPoweredResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetPoweredResponse::SetPoweredResponse()
        : ControllerToHostPacket(Packet::Id::SetPoweredResponse, initial_type(), initial_packet_code(), final_packet_code()) {
      m_state = false;
    }

    void SetPoweredResponse::unserialize(MGMTFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());

      if (m_status == BaBLE::StatusCode::Success) {
        auto m_current_settings = extractor.get_value<uint32_t>();
        m_state = (m_current_settings & 1) > 0;
      }
    }

    vector<uint8_t> SetPoweredResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateSetPowered(builder, m_state);

      return builder.build(payload, BaBLE::Payload::SetPowered);
    }

    const string SetPoweredResponse::stringify() const {
      stringstream result;

      result << "<SetPoweredResponse> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
