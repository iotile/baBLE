#include "SetConnectableRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetConnectableRequest::SetConnectableRequest(bool state)
        : HostToControllerPacket(Packet::Id::SetConnectableRequest, final_type(), final_packet_code()) {
      m_state = state;
    }

    void SetConnectableRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::SetConnectable*>();

      m_state = payload->state();
    }

    vector<uint8_t> SetConnectableRequest::serialize(MGMTFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);
      builder.add(m_state);

      return builder.build();
    }

    const string SetConnectableRequest::stringify() const {
      stringstream result;

      result << "<SetConnectableRequest> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
