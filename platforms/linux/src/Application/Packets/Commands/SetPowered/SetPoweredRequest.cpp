#include "SetPoweredRequest.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetPoweredRequest::SetPoweredRequest(Packet::Type initial_type, Packet::Type translated_type)
        : RequestPacket(initial_type, translated_type) {
      m_id = Packet::Id::SetPoweredRequest;
      m_state = false;
    }

    void SetPoweredRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::SetPowered*>();

      m_state = payload->state();
    }

    vector<uint8_t> SetPoweredRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      builder.add(m_state);

      return builder.build();
    }

    const std::string SetPoweredRequest::stringify() const {
      stringstream result;

      result << "<SetPoweredRequest> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state);

      return result.str();
    }

  }

}
