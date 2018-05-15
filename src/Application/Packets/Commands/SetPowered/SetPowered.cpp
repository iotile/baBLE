#include "SetPowered.hpp"

using namespace std;

namespace Packet::Commands {

  SetPowered::SetPowered(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_state = false;
  };

  void SetPowered::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    try {
      m_state = static_cast<bool>(stoi(extractor.get()));

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Missing arguments for 'SetPowered' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<state>", m_uuid_request);
    } catch (const std::bad_cast& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'SetPowered' packet. Can't cast.", m_uuid_request);
    } catch (const std::invalid_argument& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'SetPowered' packet.", m_uuid_request);
    }
  };

  void SetPowered::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const Schemas::SetPowered*>(Schemas::Payload::SetPowered);

    m_state = payload->state();
  };

  void SetPowered::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_native_status == 0){
      auto m_current_settings = extractor.get_value<uint32_t>();
      m_state = (m_current_settings & 1) > 0;
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

    auto payload = Schemas::CreateSetPowered(builder, m_state);

    return builder.build(payload, Schemas::Payload::SetPowered);
  }

  vector<uint8_t> SetPowered::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    builder.add(m_state);
    return builder.build();
  };

}
