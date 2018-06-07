#include <sstream>
#include "SetDiscoverableResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetDiscoverableResponse::SetDiscoverableResponse()
        : ControllerToHostPacket(Packet::Id::SetDiscoverableResponse, initial_type(), initial_packet_code(), final_packet_code()) {
      m_state = false;
    }

    void SetDiscoverableResponse::unserialize(MGMTFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());

      if (m_status == BaBLE::StatusCode::Success) {
        auto m_current_settings = extractor.get_value<uint32_t>();
        m_state = (m_current_settings & 1 << 3) > 0;
      }
    }

    vector<uint8_t> SetDiscoverableResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateSetDiscoverable(builder, m_state);

      return builder.build(payload, BaBLE::Payload::SetDiscoverable);
    }

    const std::string SetDiscoverableResponse::stringify() const {
      stringstream result;

      result << "<SetDiscoverableResponse> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
