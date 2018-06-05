#include "CreateConnection.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    CreateConnection::CreateConnection(const std::string& address, uint8_t address_type)
        : HostToControllerPacket(Packet::Id::CreateConnection, final_type(), final_packet_code(), false) {
      m_scan_interval = 0x0060; // 60ms
      m_scan_window = 0x0060; // 60ms;
      m_policy = 0x00; // Use peer address
      m_peer_address_type = address_type;
      m_address = address;
      m_own_address_type = 0x00; // Public address type
      m_connection_interval_min = 0x0018; // 30ms
      m_connection_interval_max = 0x0028; // 50ms
      m_connection_latency = 0x0000;
      m_supervision_timeout = 0x002A; // 0.42s
      m_min_ce_length = 0x0000;
      m_max_ce_length = 0x0000;

      m_raw_address = Utils::extract_bd_address(m_address);
    }

    void CreateConnection::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::Connect*>();

      m_peer_address_type = payload->address_type();
      m_address = payload->address()->str();

      try {
        m_raw_address = Utils::extract_bd_address(m_address);

      } catch (const Exceptions::WrongFormatException& err) {
        throw Exceptions::InvalidCommandException("Wrong MAC address given.", m_uuid_request);
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

  }

}
