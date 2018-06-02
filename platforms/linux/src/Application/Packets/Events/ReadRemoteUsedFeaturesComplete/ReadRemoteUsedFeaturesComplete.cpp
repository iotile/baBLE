#include "ReadRemoteUsedFeaturesComplete.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    ReadRemoteUsedFeaturesComplete::ReadRemoteUsedFeaturesComplete(Packet::Type initial_type,
                                                                       Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::ReadRemoteUsedFeaturesComplete;
      m_le_encryption = false;
    }

    void ReadRemoteUsedFeaturesComplete::unserialize(HCIFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());
      m_connection_id = extractor.get_value<uint16_t>();
      m_le_encryption = (extractor.get_value<uint8_t>() & 0x01) > 0;
    }

    vector<uint8_t> ReadRemoteUsedFeaturesComplete::serialize(AsciiFormatBuilder& builder) const {
      EventPacket::serialize(builder);
      builder
          .set_name("ReadRemoteUsedFeaturesComplete")
          .add("LE encryption", m_le_encryption);

      return builder.build();
    }

  }

}
