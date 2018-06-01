#include "SetDiscoverableRequest.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet::Commands {

  SetDiscoverableRequest::SetDiscoverableRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::SetDiscoverableRequest;
    m_state = 0;
    m_timeout = 0;
  }

  void SetDiscoverableRequest::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_state = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());
      m_timeout = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());

      if (m_state < 0 || m_state > 2) {
        throw Exceptions::InvalidCommandException("Invalid value for state for 'SetDiscoverable' packet.", m_uuid_request);
      }
      if (m_state == 2 && m_timeout == 0) {
        throw Exceptions::InvalidCommandException("Timeout is required if state = 2 for 'SetDiscoverable' packet.", m_uuid_request);
      }

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'SetDiscoverable' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,"
                                                       "<state>", m_uuid_request);
    }
  }

  void SetDiscoverableRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
    auto payload = extractor.get_payload<const BaBLE::SetDiscoverable*>();

    m_state = static_cast<uint8_t>(payload->state());
    m_timeout = payload->timeout();
  }

  vector<uint8_t> SetDiscoverableRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder
        .set_name("SetDiscoverable")
        .add("State", m_state)
        .add("Timeout", m_timeout);

    return builder.build();
  }

  vector<uint8_t> SetDiscoverableRequest::serialize(MGMTFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder
        .add(m_state)
        .add(m_timeout);

    return builder.build();
  }

}
