#include "Exit.hpp"

using namespace std;

namespace Packet {

  namespace Control {

    Exit::Exit(Packet::Type initial_type, Packet::Type final_type)
        : AbstractPacket(initial_type, final_type) {
      m_id = Packet::Id::Exit;
      m_packet_code = packet_code(m_current_type);
    }

    void Exit::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> Exit::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateExit(builder);

      return builder.build(payload, BaBLE::Payload::Exit);
    }

    const std::string Exit::stringify() const {
      stringstream result;

      result << "<Exit> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}