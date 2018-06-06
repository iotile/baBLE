#include "StartScan.hpp"
#include "../../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"

using namespace std;

namespace Packet {

  namespace Meta {

    StartScan::StartScan(bool active_scan)
        : HostOnlyPacket(Packet::Id::StartScan, initial_packet_code()) {
      m_set_scan_params_packet = std::make_shared<Packet::Commands::SetScanParameters>(active_scan);
      m_set_scan_enable_packet = std::make_shared<Packet::Commands::SetScanEnable>(true);

      m_waiting_response = Packet::Id::SetScanParameters;

      m_active_scan = active_scan;
    }

    void StartScan::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::StartScan*>();

      m_active_scan = payload->active_scan();

      m_set_scan_params_packet->set_scan_type(m_active_scan);
      m_set_scan_params_packet->set_controller_id(m_controller_id);
      m_set_scan_enable_packet->set_controller_id(m_controller_id);
    }

    vector<uint8_t> StartScan::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateStartScan(builder, m_active_scan);

      return builder.build(payload, BaBLE::Payload::StartScan);
    }

    vector<uint8_t> StartScan::serialize(HCIFormatBuilder& builder) const {
      switch (m_waiting_response) {
        case Packet::Id::SetScanParameters:
          return m_set_scan_params_packet->serialize(builder);

        case Packet::Id::SetScanEnable:
          return m_set_scan_enable_packet->serialize(builder);

        default:
          throw std::runtime_error("Can't serialize 'StartScan' to HCI.");
      }
    }

    const std::string StartScan::stringify() const {
      stringstream result;

      result << "<StartScan> "
             << AbstractPacket::stringify() << ", "
             << "Active scan: " << to_string(m_active_scan);

      return result.str();
    }

    void StartScan::prepare(const std::shared_ptr<PacketRouter>& router) {
      switch (m_waiting_response) {
        case Packet::Id::SetScanParameters: {
          m_set_scan_params_packet->translate();
          m_current_type = m_set_scan_params_packet->get_type();

          PacketUuid uuid = m_set_scan_params_packet->get_response_uuid();
          auto callback =
              [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
                return on_set_scan_params_response_received(router, packet);
              };
          router->add_callback(uuid, shared_from(this), callback);
          break;
        }

        case Packet::Id::SetScanEnable: {
          m_set_scan_enable_packet->translate();
          m_current_type = m_set_scan_enable_packet->get_type();

          PacketUuid uuid = m_set_scan_enable_packet->get_response_uuid();
          auto callback =
              [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
                return on_set_scan_enable_response_received(router, packet);
              };
          router->add_callback(uuid, shared_from(this), callback);
          break;
        }

        default:
          m_current_type = final_type();
          break;
      }
    }

    shared_ptr<AbstractPacket> StartScan::on_set_scan_params_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                               const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Set scan params response received", "StartScan");

      m_set_scan_params_packet = dynamic_pointer_cast<Packet::Commands::SetScanParameters>(
          m_set_scan_params_packet->on_response_received(router, packet)
      );
      if (m_set_scan_params_packet == nullptr) {
        throw Exceptions::RuntimeErrorException("Can't cast AbstractPacket to SetScanParameter packet");
      }

      if (m_set_scan_params_packet->get_status() != BaBLE::StatusCode::Success) {
        import_status(m_set_scan_params_packet);
        m_waiting_response = Packet::Id::None;
      } else {
        m_waiting_response = Packet::Id::SetScanEnable;
      }

      return shared_from(this);
    }

    shared_ptr<AbstractPacket> StartScan::on_set_scan_enable_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                               const std::shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Set scan enable response received", "StartScan");
      m_set_scan_enable_packet = dynamic_pointer_cast<Packet::Commands::SetScanEnable>(
          m_set_scan_enable_packet->on_response_received(router, packet)
      );
      if (m_set_scan_enable_packet == nullptr) {
        throw Exceptions::RuntimeErrorException("Can't cast AbstractPacket to SetScanEnable packet");
      }

      import_status(m_set_scan_params_packet);
      m_waiting_response = Packet::Id::None;

      return shared_from(this);
    }

  }

}