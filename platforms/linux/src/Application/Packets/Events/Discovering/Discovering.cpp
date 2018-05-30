#include "Discovering.hpp"

using namespace std;

namespace Packet::Events {

  Discovering::Discovering(Packet::Type initial_type, Packet::Type translated_type)
  : EventPacket(initial_type, translated_type) {
    m_id = Packet::Id::Discovering;
    m_address_type = 0;
    m_discovering = 0;
  };

  void Discovering::unserialize(MGMTFormatExtractor& extractor) {
    m_address_type = extractor.get_value<uint8_t>();
    m_discovering = extractor.get_value<uint8_t>();
  };

  vector<uint8_t> Discovering::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);
    builder
        .set_name("Discovering")
        .add("Address type", m_address_type)
        .add("Discovering", m_discovering);

    return builder.build();
  };

  vector<uint8_t> Discovering::serialize(FlatbuffersFormatBuilder& builder) const {
    auto payload = BaBLE::CreateDiscovering(builder, m_address_type, m_discovering);

    return builder.build(payload, BaBLE::Payload::Discovering);
  }

}
