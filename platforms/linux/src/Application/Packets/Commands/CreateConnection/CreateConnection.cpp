#include "CreateConnection.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    CreateConnection::CreateConnection(const std::string& address, uint8_t address_type)
        : HostToControllerPacket(Packet::Id::CreateConnection, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::EventCode::CommandStatus;

      m_scan_interval = 0x0060; // 60ms
      m_scan_window = 0x0060; // 60ms;
      m_policy = 0x00; // Use peer address
      m_peer_address_type = address_type;
      m_address = address;
      m_own_address_type = 0x00; // Public address type
      m_connection_interval_min = 0x0006; // 7.5ms
      m_connection_interval_max = 0x0018; // 30ms
      m_connection_latency = 0x0000;
      m_supervision_timeout = 0x002A; // 0.42s
      m_min_ce_length = 0x0000;
      m_max_ce_length = 0x0000;

      m_raw_address = Utils::extract_bd_address(m_address);
      m_response_received = false;
    }

    void CreateConnection::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::Connect*>();

      m_peer_address_type = payload->address_type();
      m_address = payload->address()->str();

      try {
        m_raw_address = Utils::extract_bd_address(m_address);

      } catch (const Exceptions::BaBLEException& err) {
        throw Exceptions::BaBLEException(BaBLE::StatusCode::InvalidCommand, "Wrong MAC address given.", m_uuid_request);
      }
    }

    vector<uint8_t> CreateConnection::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_scan_interval)
          .add(m_scan_window)
          .add(m_policy)
          .add(m_peer_address_type)
          .add(m_raw_address)
          .add(m_own_address_type)
          .add(m_connection_interval_min)
          .add(m_connection_interval_max)
          .add(m_connection_latency)
          .add(m_supervision_timeout)
          .add(m_min_ce_length)
          .add(m_max_ce_length);

      return builder.build(Format::HCI::Type::Command);
    }

    vector<uint8_t> CreateConnection::serialize(FlatbuffersFormatBuilder& builder) const {
      auto address = builder.CreateString(m_address);

      auto payload = BaBLE::CreateConnect(builder, address, m_peer_address_type);

      return builder.build(payload, BaBLE::Payload::Connect);
    }

    const std::string CreateConnection::stringify() const {
      stringstream result;

      result << "<CreateConnection> "
             << AbstractPacket::stringify() << ", "
             << "Scan interval: " << to_string(m_scan_interval) << ", "
             << "Scan window: " << to_string(m_scan_window) << ", "
             << "Policy: " << to_string(m_policy) << ", "
             << "Peer address type: " << to_string(m_peer_address_type) << ", "
             << "Address: " << m_address << ", "
             << "Own address type: " << to_string(m_own_address_type) << ", "
             << "Connection interval: [" << to_string(m_connection_interval_min) << ", "
                                         << to_string(m_connection_interval_max) << "], "
             << "Connection latency: " << to_string(m_connection_latency) << ", "
             << "Supervision timeout: " << to_string(m_supervision_timeout) << ", "
             << "CE length: [" << to_string(m_min_ce_length) << ", "
                               << to_string(m_max_ce_length) << "]";

      return result.str();
    }

    void CreateConnection::prepare(const shared_ptr<PacketRouter>& router) {
      if (!m_response_received) {
        m_current_type = final_type();

        PacketUuid response_uuid = get_response_uuid();
        response_uuid.connection_handle = 0x0000;
        auto response_callback =
            [this](const shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
              return on_response_received(router, packet);
            };
        router->add_callback(response_uuid, shared_from(this), response_callback);
      } else {
        m_current_type = initial_type();
      }
    }

    shared_ptr<AbstractPacket> CreateConnection::on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                              const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Response received", "CreateConnection");
      import_status(packet);
      m_response_received = true;

      return shared_from(this);
    }

  }

}
