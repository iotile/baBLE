#include "SetDiscoverableRequest.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetDiscoverableRequest::SetDiscoverableRequest(bool state, uint16_t timeout)
        : HostToControllerPacket(Packet::Id::SetDiscoverableRequest, final_type(), final_packet_code()) {
      m_state = state;
      m_timeout = timeout;
    }

    void SetDiscoverableRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::SetDiscoverable*>();

      m_state = payload->state();
      m_timeout = payload->timeout();
    }

    vector<uint8_t> SetDiscoverableRequest::serialize(MGMTFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);
      builder
          .add(m_state)
          .add(m_timeout);

      return builder.build();
    }

    const string SetDiscoverableRequest::stringify() const {
      stringstream result;

      result << "<SetDiscoverableRequest> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state) << ", "
             << "Timeout: " << to_string(m_timeout);

      return result.str();
    }

  }

}
