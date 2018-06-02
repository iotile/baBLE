#include "SetScanParameters.hpp"
#include "../../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"
#include "../../Events/CommandComplete/CommandComplete.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetScanParameters::SetScanParameters(Packet::Type initial_type, Packet::Type translated_type)
        : RequestPacket(initial_type, translated_type) {
      m_id = Packet::Id::ReadRequest;
      m_response_packet_code = Format::HCI::EventCode::CommandComplete;

      m_scan_type = 0x01; // Active
      m_scan_interval = 0x0012; // 11.25msec
      m_scan_window = 0x0012; // 11.25 msec
      m_address_type = 0x01; // Random
      m_policy = 0x00; // Accept all advertisements. Ignore directed advertisements not addressed to this device.
    }

    void SetScanParameters::set_scan_type(bool active) {
      if (active) {
        m_scan_type = 0x01;
      } else {
        m_scan_type = 0x00;
      }
    }

    void SetScanParameters::unserialize(AsciiFormatExtractor& extractor) {
      try {
        m_scan_type = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());
        m_scan_interval = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());
        m_scan_window = AsciiFormat::string_to_number<uint16_t>(extractor.get_string());
        m_address_type = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());
        m_policy = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());

      } catch (const Exceptions::WrongFormatException& err) {
        throw Exceptions::InvalidCommandException("Invalid arguments for 'SetScanParameters' packet."
                                                  "Usage: <uuid>,<command_code>,<controller_id>,"
                                                  "<scan_type>,<scan_interval>,<scan_window>,<address_type>,<policy>", m_uuid_request);
      }
    }

    vector<uint8_t> SetScanParameters::serialize(AsciiFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      builder
          .set_name("SetScanParameters")
          .add("Scan type", m_scan_type)
          .add("Scan interval", m_scan_interval)
          .add("Scan window", m_scan_window)
          .add("Address type", m_address_type)
          .add("Policy", m_policy);

      return builder.build();
    }

    vector<uint8_t> SetScanParameters::serialize(HCIFormatBuilder& builder) const {
      RequestPacket::serialize(builder);

      builder
          .add(m_scan_type)
          .add(m_scan_interval)
          .add(m_scan_window)
          .add(m_address_type)
          .add(m_policy);

      return builder.build(Format::HCI::Type::Command);
    }

    shared_ptr<Packet::AbstractPacket> SetScanParameters::on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                         const shared_ptr<Packet::AbstractPacket>& packet) {
      LOG.debug("Response received", "SetScanParameters");
      auto response_packet = dynamic_pointer_cast<Packet::Events::CommandComplete>(packet);
      if (response_packet == nullptr) {
        throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to CommandComplete packet.");
      }

      vector<uint8_t> result = response_packet->get_returned_params();
      if (result.empty()) {
        throw Exceptions::WrongFormatException("Response received with wrong result format (SetScanParameters).", m_uuid_request);
      }

      set_status(result.at(0));

      return shared_from(this);
    }

  }

}