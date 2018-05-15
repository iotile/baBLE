#include "ControllerAdded.hpp"

using namespace std;

namespace Packet::Events {

  ControllerAdded::ControllerAdded(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {}

  void ControllerAdded::unserialize(MGMTFormatExtractor& extractor) {
    EventPacket::unserialize(extractor);
  };

  vector<uint8_t> ControllerAdded::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    builder.set_name("ControllerAdded");

    return builder.build();
  };

  vector<uint8_t> ControllerAdded::serialize(FlatbuffersFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    auto payload = Schemas::CreateControllerAdded(builder);

    return builder.build(m_controller_id, payload, Schemas::Payload::ControllerAdded, m_native_class);
  }

}