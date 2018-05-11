#include "SetPowered.hpp"

using namespace std;

namespace Packet::Commands {

  SetPowered::SetPowered(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_state = false;
  };

  void SetPowered::import(AsciiFormatExtractor& extractor) {
    CommandPacket::import(extractor);

    try {
      m_controller_id = static_cast<uint16_t>(stoi(extractor.get()));
      m_state = static_cast<uint8_t>(stoi(extractor.get()));

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Missing arguments for 'SetPowered' packet. Usage: <command_code>,<controller_id>,<state>");
    }
  };

  void SetPowered::import(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::import(extractor);
    auto payload = extractor.get_payload<const Schemas::SetPowered*>(Schemas::Payload::SetPowered);

    m_controller_id = payload->controller_id();
    m_state = payload->state();
  };

  void SetPowered::import(MGMTFormatExtractor& extractor) {
    CommandPacket::import(extractor);

    if (m_native_status == 0){
      auto m_current_settings = extractor.get_value<uint32_t>();
      m_state = (m_current_settings & 1) == 1;
    }
  };

  vector<uint8_t> SetPowered::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("SetPowered")
        .add("State", m_state);

    return builder.build();
  };

  vector<uint8_t> SetPowered::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto payload = Schemas::CreateSetPowered(builder, m_controller_id, m_state);

    return builder.build(payload, Schemas::Payload::SetPowered, m_native_class, m_status, m_native_status);
  }

  vector<uint8_t> SetPowered::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    builder.add(m_state);
    return builder.build();
  };

}
