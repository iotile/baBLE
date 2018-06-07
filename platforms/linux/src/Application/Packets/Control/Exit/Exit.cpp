#include "Exit.hpp"

using namespace std;

namespace Packet {

  namespace Control {

    Exit::Exit(Packet::Type initial_type, Packet::Type translated_type)
        : AbstractPacket(initial_type, translated_type) {
      m_id = Packet::Id::Exit;
      m_packet_code = packet_code(m_current_type);
    }

    void Exit::unserialize(AsciiFormatExtractor& extractor) {}

    void Exit::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> Exit::serialize(AsciiFormatBuilder& builder) const {
      builder.set_name("Exit");
      return {};
    };

    vector<uint8_t> Exit::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateExit(builder);

      return builder.build(payload, BaBLE::Payload::Exit);
    };

  }

}