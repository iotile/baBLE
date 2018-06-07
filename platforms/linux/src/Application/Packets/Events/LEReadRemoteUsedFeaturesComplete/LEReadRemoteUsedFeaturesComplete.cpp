#include "LEReadRemoteUsedFeaturesComplete.hpp"

using namespace std;

namespace Packet::Events {

  LEReadRemoteUsedFeaturesComplete::LEReadRemoteUsedFeaturesComplete(Packet::Type initial_type, Packet::Type translated_type)
      : EventPacket(initial_type, translated_type) {
    m_id = BaBLE::Payload::NONE;
    m_connection_handle = 0;
    m_le_encryption = false;
  }

  void LEReadRemoteUsedFeaturesComplete::unserialize(HCIFormatExtractor& extractor) {
    EventPacket::unserialize(extractor);
    set_status(extractor.get_value<uint8_t>());
    m_connection_handle = extractor.get_value<uint16_t>();
    m_le_encryption = (extractor.get_value<uint8_t>() & 0x01) > 0;
  }

  vector<uint8_t> LEReadRemoteUsedFeaturesComplete::serialize(AsciiFormatBuilder& builder) const {
    EventPacket::serialize(builder);
    builder
        .set_name("LEReadRemoteUsedFeaturesComplete")
        .add("Connection handle", m_connection_handle)
        .add("LE encryption", m_le_encryption);

    return builder.build();
  }

}
