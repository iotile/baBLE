#include "ControllerRemoved.hpp"

using namespace std;

namespace Packet::Events {

  ControllerRemoved::ControllerRemoved(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {
    m_id = Packet::Id::ControllerRemoved;
  }

  void ControllerRemoved::unserialize(MGMTFormatExtractor& extractor) {};

  vector<uint8_t> ControllerRemoved::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);

    builder.set_name("ControllerRemoved");

    return builder.build();
  };

  vector<uint8_t> ControllerRemoved::serialize(FlatbuffersFormatBuilder& builder) const {
    auto payload = BaBLE::CreateControllerRemoved(builder);

    return builder.build(payload, BaBLE::Payload::ControllerRemoved);
  }

}