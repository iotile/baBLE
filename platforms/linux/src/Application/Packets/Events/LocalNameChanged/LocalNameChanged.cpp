#include "LocalNameChanged.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    LocalNameChanged::LocalNameChanged(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::LocalNameChanged;
    };

    void LocalNameChanged::unserialize(MGMTFormatExtractor& extractor) {
      m_name = extractor.get_array<uint8_t, 249>();
      m_short_name = extractor.get_array<uint8_t, 11>();
    };

    vector<uint8_t> LocalNameChanged::serialize(AsciiFormatBuilder& builder) const {
      EventPacket::serialize(builder);
      builder
          .set_name("LocalNameChanged")
          .add("Name", AsciiFormat::bytes_to_string(m_name))
          .add("Short name", AsciiFormat::bytes_to_string(m_short_name));

      return builder.build();
    };

  }

}
