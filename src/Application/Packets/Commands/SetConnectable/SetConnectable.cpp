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
      m_state = static_cast<bool>(stoi(extractor.get()));

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Missing arguments for 'SetConnectable' packet. Usage: <command_code>,<controller_id>,<state>");
    } catch (const std::bad_cast& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'SetConnectable' packet. Can't cast.");
    } catch (const std::invalid_argument& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'SetConnectable' packet.");
    }
  }

  void SetConnectable::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const Schemas::SetConnectable*>(Schemas::Payload::SetConnectable);

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

    return builder.build(m_controller_id, payload, Schemas::Payload::SetConnectable, m_native_class, m_status, m_native_status);
  }

  vector<uint8_t> SetConnectable::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    builder
        .add(m_state);
    return builder.build();
  }

}
