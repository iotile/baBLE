#include "Disconnect.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    Disconnect::Disconnect(Packet::Type initial_type, Packet::Type final_type)
        : RequestPacket(initial_type, final_type) {
      m_id = Packet::Id::Disconnect;
      m_reason = Format::HCI::StatusCode::ConnectionTerminatedLocalHost;
    }

    void Disconnect::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::Disconnect*>();

      m_connection_id = payload->connection_handle();
    }

    vector<uint8_t> Disconnect::serialize(HCIFormatBuilder& builder) const {
      RequestPacket::serialize(builder);

      builder
          .add(m_connection_id)
          .add(m_reason);
      return builder.build(Format::HCI::Type::Command);
    }

    const std::string Disconnect::stringify() const {
      stringstream result;

      result << "<Disconnect> "
             << AbstractPacket::stringify() << ", "
             << "Reason: " << to_string(m_reason);

      return result.str();
    }

    void Disconnect::before_sent(const std::shared_ptr<PacketRouter>& router) {
      AbstractPacket::before_sent(router);
      m_packet_code = packet_code(m_current_type);
    }

  }

}
