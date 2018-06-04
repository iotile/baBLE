#include "SetConnectableRequest.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetConnectableRequest::SetConnectableRequest(Packet::Type initial_type, Packet::Type final_type)
        : RequestPacket(initial_type, final_type) {
      m_id = Packet::Id::SetConnectableRequest;
      m_state = false;
    }

    void SetConnectableRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::SetConnectable*>();

      m_state = payload->state();
    }

    vector<uint8_t> SetConnectableRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      builder.add(m_state);

      return builder.build();
    }

    const std::string SetConnectableRequest::stringify() const {
      stringstream result;

      result << "<SetConnectableRequest> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
