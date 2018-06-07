#include "Disconnect.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    Disconnect::Disconnect(uint8_t reason)
        : HostToControllerPacket(Packet::Id::Disconnect, final_type(), final_packet_code(), false) {
      m_reason = reason;
    }

    void Disconnect::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::Disconnect*>();

      m_connection_id = payload->connection_handle();
    }

    vector<uint8_t> Disconnect::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_connection_id)
          .add(m_reason);

      return builder.build(Format::HCI::Type::Command);
    }

    const string Disconnect::stringify() const {
      stringstream result;

      result << "<Disconnect> "
             << AbstractPacket::stringify() << ", "
             << "Reason: " << to_string(m_reason);

      return result.str();
    }

  }

}
