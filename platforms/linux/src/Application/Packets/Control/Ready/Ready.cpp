#include "Ready.hpp"

using namespace std;

namespace Packet {

  namespace Control {

    Ready::Ready(Packet::Type output_type)
        : AbstractPacket(output_type, output_type) {
      m_id = Packet::Id::Ready;
      m_packet_code = static_cast<uint16_t>(BaBLE::Payload::Ready);
      m_native_class = "BaBLE";
    }

    vector<uint8_t> Ready::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateReady(builder);

      return builder.build(payload, BaBLE::Payload::Ready);
    }

    const std::string Ready::stringify() const {
      stringstream result;

      result << "<Ready> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}