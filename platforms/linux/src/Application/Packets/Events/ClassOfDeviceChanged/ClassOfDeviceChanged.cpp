#include "ClassOfDeviceChanged.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    ClassOfDeviceChanged::ClassOfDeviceChanged(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::ClassOfDeviceChanged;
    };

    void ClassOfDeviceChanged::unserialize(MGMTFormatExtractor& extractor) {
      m_class_of_device = extractor.get_array<uint8_t, 3>();
    };

    vector<uint8_t> ClassOfDeviceChanged::serialize(AsciiFormatBuilder& builder) const {
      EventPacket::serialize(builder);
      builder
          .set_name("ClassOfDeviceChanged")
          .add("Class of Device", m_class_of_device);

      return builder.build();
    };

  }

}
