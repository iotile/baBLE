#include "LEReadRemoteUsedFeaturesComplete.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    LEReadRemoteUsedFeaturesComplete::LEReadRemoteUsedFeaturesComplete(Packet::Type initial_type,
                                                                       Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::LEReadRemoteUsedFeaturesComplete;
      m_le_encryption = false;
    }

    void LEReadRemoteUsedFeaturesComplete::unserialize(HCIFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());
      m_connection_id = extractor.get_value<uint16_t>();
      m_le_encryption = (extractor.get_value<uint8_t>() & 0x01) > 0;
    }

    vector<uint8_t> LEReadRemoteUsedFeaturesComplete::serialize(AsciiFormatBuilder& builder) const {
      EventPacket::serialize(builder);
      builder
          .set_name("LEReadRemoteUsedFeaturesComplete")
          .add("LE encryption", m_le_encryption);

      return builder.build();
    }

  }

}
