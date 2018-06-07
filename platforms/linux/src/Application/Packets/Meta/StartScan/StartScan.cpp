#include "StartScan.hpp"
#include "../../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet {

  namespace Meta {

    StartScan::StartScan(Packet::Type initial_type, Packet::Type translated_type)
        : AbstractPacket(initial_type, translated_type) {
      m_id = Packet::Id::StartScan;
      m_packet_code = packet_code(m_current_type);

      m_set_scan_params_packet = std::make_shared<Packet::Commands::SetScanParameters>(translated_type, translated_type);
      m_set_scan_enable_packet = std::make_shared<Packet::Commands::SetScanEnable>(translated_type, translated_type);
      m_set_scan_enable_packet->set_state(true);

      m_waiting_response = SubPacket::SetScanParameters;
      m_current_index = 0;

      m_active_scan = true;
    }

    void StartScan::unserialize(AsciiFormatExtractor& extractor) {
      try {
        m_active_scan = AsciiFormat::string_to_number<bool>(extractor.get_string());

      } catch (const Exceptions::WrongFormatException& err) {
        throw Exceptions::InvalidCommandException("Invalid arguments for 'StartScan' packet."
                                                  "Usage: <uuid>,<command_code>,<controller_id>,"
                                                  "<active_scan>", m_uuid_request);
      }

      m_set_scan_params_packet->set_scan_type(m_active_scan);
      m_set_scan_params_packet->set_controller_id(m_controller_id);
      m_set_scan_enable_packet->set_controller_id(m_controller_id);
    }

    void StartScan::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::StartScan*>();

      m_active_scan = payload->active_scan();

      m_set_scan_params_packet->set_scan_type(m_active_scan);
      m_set_scan_params_packet->set_controller_id(m_controller_id);
      m_set_scan_enable_packet->set_controller_id(m_controller_id);
    }

    vector<uint8_t> StartScan::serialize(AsciiFormatBuilder& builder) const {
      builder
          .set_name("StartScan")
          .add("Type", "Meta")
          .add("Active", m_active_scan);

      return builder.build();
    }

    vector<uint8_t> StartScan::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateStartScan(builder, m_active_scan);

      return builder.build(payload, BaBLE::Payload::StartScan);
    }

    vector<uint8_t> StartScan::serialize(HCIFormatBuilder& builder) const {
      switch (m_waiting_response) {
        case SubPacket::SetScanParameters:
          return m_set_scan_params_packet->serialize(builder);

        case SubPacket::SetScanEnable:
          return m_set_scan_enable_packet->serialize(builder);

        case SubPacket::None:
          throw std::runtime_error("Can't serialize 'StartScan' to HCI.");
      }
    }

    void StartScan::before_sent(const std::shared_ptr<PacketRouter>& router) {
      switch (m_waiting_response) {
        case SetScanParameters: {
          m_current_type = m_translated_type;

          PacketUuid uuid = m_set_scan_params_packet->get_response_uuid();
          auto callback =
              [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
                return on_set_scan_params_response_received(router, packet);
              };
          router->add_callback(uuid, shared_from(this), callback);
          break;
        }

        case SetScanEnable: {
          m_current_type = m_translated_type;

          PacketUuid uuid = m_set_scan_enable_packet->get_response_uuid();
          auto callback =
              [this](const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet) {
                return on_set_scan_enable_response_received(router, packet);
              };
          router->add_callback(uuid, shared_from(this), callback);
          break;
        }

        case None:
          m_current_type = m_initial_type;
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
        import_status(*m_set_scan_params_packet);
        m_waiting_response = SubPacket::None;
      } else {
        m_waiting_response = SubPacket::SetScanEnable;
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

      import_status(*m_set_scan_params_packet);
      m_waiting_response = SubPacket::None;

      return shared_from(this);
    }

  }

}