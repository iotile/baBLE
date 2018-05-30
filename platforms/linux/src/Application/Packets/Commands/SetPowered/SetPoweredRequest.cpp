#include "SetPoweredRequest.hpp"

using namespace std;

namespace Packet::Commands {

  SetPoweredRequest::SetPoweredRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::SetPoweredRequest;
    m_state = false;
  }

  void SetPoweredRequest::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_state = AsciiFormat::string_to_number<bool>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'SetPowered' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,"
                                                       "<state>", m_uuid_request);
    }
  }

  void SetPoweredRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
    auto payload = extractor.get_payload<const BaBLE::SetPowered*>();

    m_state = payload->state();
  }

  vector<uint8_t> SetPoweredRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder
        .set_name("SetPowered")
        .add("State", m_state);

    return builder.build();
  }

  vector<uint8_t> SetPoweredRequest::serialize(MGMTFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder.add(m_state);

    return builder.build();
  }

}
