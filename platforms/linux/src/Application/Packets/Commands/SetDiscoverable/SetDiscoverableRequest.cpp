#include "SetDiscoverableRequest.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetDiscoverableRequest::SetDiscoverableRequest(Packet::Type initial_type, Packet::Type final_type)
        : RequestPacket(initial_type, final_type) {
      m_id = Packet::Id::SetDiscoverableRequest;
      m_state = 0;
      m_timeout = 0;
    }

    void SetDiscoverableRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::SetDiscoverable*>();

      m_state = static_cast<uint8_t>(payload->state());
      m_timeout = payload->timeout();
    }

    vector<uint8_t> SetDiscoverableRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      builder
          .add(m_state)
          .add(m_timeout);

      return builder.build();
    }

    const std::string SetDiscoverableRequest::stringify() const {
      stringstream result;

      result << "<SetDiscoverableRequest> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state) << ", "
             << "Timeout: " << to_string(m_timeout);

      return result.str();
    }

  }

}
