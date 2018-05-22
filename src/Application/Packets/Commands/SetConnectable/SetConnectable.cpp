#include "SetConnectable.hpp"

using namespace std;

namespace Packet::Commands {

  SetConnectable::SetConnectable(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_state = false;
  }

  void SetConnectable::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    try {
      m_state = AsciiFormat::string_to_number<bool>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'SetConnectable' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<state>", m_uuid_request);
    }
  }

  void SetConnectable::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const Schemas::SetConnectable*>();

    m_state = payload->state();
  }

  void SetConnectable::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_native_status == 0){
      auto m_current_settings = extractor.get_value<uint32_t>();
      m_state = (m_current_settings & 1 << 1) > 0;
    }
  }

  vector<uint8_t> SetConnectable::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("SetConnectable")
        .add("State", m_state);

    return builder.build();
  }

  vector<uint8_t> SetConnectable::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto payload = Schemas::CreateSetConnectable(builder, m_state);

    return builder.build(payload, Schemas::Payload::SetConnectable);
  }

  vector<uint8_t> SetConnectable::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    builder
        .add(m_state);
    return builder.build();
  }

}
