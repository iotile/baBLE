#include "StopScan.hpp"
#include "../../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"

using namespace std;

namespace Packet {

  namespace Meta {

    StopScan::StopScan()
        : HostOnlyPacket(Packet::Id::StopScan, initial_packet_code()) {
      m_set_scan_enable_packet = std::make_shared<Packet::Commands::SetScanEnable>(false);

      m_waiting_response = true;
    }

    void StopScan::unserialize(FlatbuffersFormatExtractor& extractor) {
      m_set_scan_enable_packet->set_controller_id(m_controller_id);
    }

    vector<uint8_t> StopScan::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateStopScan(builder);

      return builder.build(payload, BaBLE::Payload::StopScan);
    }

    vector<uint8_t> StopScan::serialize(HCIFormatBuilder& builder) const {
      if (m_waiting_response) {
        return m_set_scan_enable_packet->serialize(builder);
      } else {
        throw std::runtime_error("Can't serialize 'StopScan' to HCI.");
      }
    }

    const std::string StopScan::stringify() const {
      stringstream result;

      result << "<StopScan> "
             << AbstractPacket::stringify();

      return result.str();
    }

    void StopScan::prepare(const std::shared_ptr<PacketRouter>& router) {
      if (m_waiting_response) {
        m_set_scan_enable_packet->translate();
        m_current_type = m_set_scan_enable_packet->get_type();

        PacketUuid uuid = m_set_scan_enable_packet->get_response_uuid();
        auto callback =
            [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
              return on_set_scan_enable_response_received(router, packet);
            };
        router->add_callback(uuid, shared_from(this), callback);
      } else {
        m_current_type = final_type();
      }
    }

    shared_ptr<AbstractPacket> StopScan::on_set_scan_enable_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                               const std::shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Set scan enable response received", "StopScan");
      m_set_scan_enable_packet = dynamic_pointer_cast<Packet::Commands::SetScanEnable>(
          m_set_scan_enable_packet->on_response_received(router, packet)
      );
      if (m_set_scan_enable_packet == nullptr) {
        throw Exceptions::RuntimeErrorException("Can't cast AbstractPacket to SetScanEnable packet", m_uuid_request);
      }

      import_status(m_set_scan_enable_packet);
      m_waiting_response = false;

      return shared_from(this);
    }

  }

}