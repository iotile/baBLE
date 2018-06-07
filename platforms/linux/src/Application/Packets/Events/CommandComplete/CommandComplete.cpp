#include "CommandComplete.hpp"

using namespace std;

namespace Packet {

  namespace Events {

    CommandComplete::CommandComplete(Packet::Type initial_type, Packet::Type translated_type)
        : EventPacket(initial_type, translated_type) {
      m_id = Packet::Id::CommandComplete;
      m_opcode = 0;
    }

    void CommandComplete::unserialize(HCIFormatExtractor& extractor) {
      auto number_allowed_command_packets = extractor.get_value<uint8_t>();
      m_opcode = extractor.get_value<uint16_t>();
      m_returned_params = extractor.get_vector<uint8_t>();
    };

    vector<uint8_t> CommandComplete::serialize(AsciiFormatBuilder& builder) const {
      EventPacket::serialize(builder);

      builder.set_name("CommandComplete");

      return builder.build();
    };

  }

}