#include "CommandComplete.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    CommandComplete::CommandComplete(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::CommandComplete;
    }

    void CommandComplete::unserialize(HCIFormatExtractor& extractor) {};

    vector<uint8_t> CommandComplete::serialize(AsciiFormatBuilder& builder) const {
      EventPacket::serialize(builder);

      builder.set_name("CommandComplete");

      return builder.build();
    };

  }

}