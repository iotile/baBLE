#include "SetPoweredRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetPoweredRequest::SetPoweredRequest(bool state)
        : HostToControllerPacket(Packet::Id::SetPoweredRequest, final_type(), final_packet_code()) {
      m_state = state;
    }

    void SetPoweredRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::SetPowered*>();

      m_state = payload->state();
    }

    vector<uint8_t> SetPoweredRequest::serialize(MGMTFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);
      builder.add(m_state);

      return builder.build();
    }

    const string SetPoweredRequest::stringify() const {
      stringstream result;

      result << "<SetPoweredRequest> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
