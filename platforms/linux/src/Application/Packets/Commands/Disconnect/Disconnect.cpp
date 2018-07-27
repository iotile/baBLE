#include "Disconnect.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    Disconnect::Disconnect(uint8_t reason)
        : HostToControllerPacket(Packet::Id::Disconnect, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::EventCode::CommandStatus;

      m_reason = reason;
      m_response_received = false;
    }

    void Disconnect::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::Disconnect*>();

      m_connection_handle = payload->connection_handle();
    }

    vector<uint8_t> Disconnect::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_connection_handle)
          .add(m_reason);

      return builder.build(Format::HCI::Type::Command);
    }

    vector<uint8_t> Disconnect::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateDisconnect(builder, m_connection_handle);

      return builder.build(payload, BaBLE::Payload::Disconnect);
    }

    const string Disconnect::stringify() const {
      stringstream result;

      result << "<Disconnect> "
             << AbstractPacket::stringify() << ", "
             << "Reason: " << to_string(m_reason);

      return result.str();
    }

    void Disconnect::prepare(const shared_ptr<PacketRouter>& router) {
      if (!m_response_received) {
        m_current_type = final_type();

        PacketUuid response_uuid = get_response_uuid();
        response_uuid.connection_handle = 0x00;
        auto response_callback =
            [this](const shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
              return on_response_received(router, packet);
            };
        router->add_callback(response_uuid, shared_from(this), response_callback);
      } else {
        m_current_type = initial_type();
      }
    }

    shared_ptr<AbstractPacket> Disconnect::on_response_received(const shared_ptr<PacketRouter>& router,
                                                                const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Response received", "Disconnect");
      import_status(packet);
      m_response_received = true;

      return shared_from(this);
    }

  }

}
