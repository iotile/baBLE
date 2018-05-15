#include "ControllerRemoved.hpp"

using namespace std;

namespace Packet::Events {

  ControllerRemoved::ControllerRemoved(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {}

  void ControllerRemoved::unserialize(MGMTFormatExtractor& extractor) {
    EventPacket::unserialize(extractor);
  };

  vector<uint8_t> ControllerRemoved::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    builder.set_name("ControllerRemoved");

    return builder.build();
  };

  vector<uint8_t> ControllerRemoved::serialize(FlatbuffersFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    auto payload = Schemas::CreateControllerRemoved(builder);

    return builder.build(m_controller_id, payload, Schemas::Payload::ControllerRemoved, m_native_class);
  }

}