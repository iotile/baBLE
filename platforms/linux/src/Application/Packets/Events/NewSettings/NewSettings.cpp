#include "NewSettings.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    NewSettings::NewSettings(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::NewSettings;
      m_current_settings = 0;
    }

    void NewSettings::unserialize(MGMTFormatExtractor& extractor) {
      m_current_settings = extractor.get_value<uint32_t>();
    }

    vector<uint8_t> NewSettings::serialize(AsciiFormatBuilder& builder) const {
      EventPacket::serialize(builder);
      builder
          .set_name("NewSettings")
          .add("Powered", (m_current_settings & 1) > 0)
          .add("Connectable", (m_current_settings & 1 << 1) > 0)
          .add("Discoverable", (m_current_settings & 1 << 3) > 0)
          .add("Low Energy", (m_current_settings & 1 << 9) > 0)
          .add("Advertising", (m_current_settings & 1 << 10) > 0);

      return builder.build();
    }

  }

}
