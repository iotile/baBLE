#include "SetScanParameters.hpp"
#include "Application/Packets/Events/CommandComplete/CommandComplete.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetScanParameters::SetScanParameters(bool active_scan, uint16_t scan_interval, uint16_t scan_window,
                                         uint8_t address_type, uint8_t policy)
        : HostToControllerPacket(Packet::Id::SetScanParameters, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::EventCode::CommandComplete;

      m_active_scan = active_scan;
      m_scan_interval = scan_interval;
      m_scan_window = scan_window;
      m_address_type = address_type;
      m_policy = policy;
    }

    void SetScanParameters::set_scan_type(bool active_scan) {
      m_active_scan = active_scan;
    }

    vector<uint8_t> SetScanParameters::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      uint8_t scan_type = 0x00;
      if (m_active_scan) {
        scan_type = 0x01;
      }

      builder
          .add(scan_type)
          .add(m_scan_interval)
          .add(m_scan_window)
          .add(m_address_type)
          .add(m_policy);

      return builder.build(Format::HCI::Type::Command);
    }

    const std::string SetScanParameters::stringify() const {
      stringstream result;

      result << "<SetScanParameters> "
             << AbstractPacket::stringify() << ", "
             << "Active scan: " << to_string(m_active_scan) << ", "
             << "Scan interval: " << to_string(m_scan_interval) << ", "
             << "Scan window: " << to_string(m_scan_window) << ", "
             << "Address type: " << to_string(m_address_type) << ", "
             << "Policy: " << to_string(m_policy);

      return result.str();
    }

    shared_ptr<Packet::AbstractPacket> SetScanParameters::on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                         const shared_ptr<Packet::AbstractPacket>& packet) {
      LOG.debug("Response received", "SetScanParameters");
      auto response_packet = dynamic_pointer_cast<Packet::Events::CommandComplete>(packet);
      if (response_packet == nullptr) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't downcast AbstractPacket to CommandComplete packet (SetScanParameters).",
            m_uuid_request
        );
      }

      vector<uint8_t> result = response_packet->get_returned_params();
      if (result.empty()) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::WrongFormat,
            "Response received with no result (SetScanParameters).",
            m_uuid_request
        );
      }

      set_status(result.at(0));
      set_waiting_response(false);
      m_final_type = Packet::Type::NONE;

      return shared_from(this);
    }

  }

}