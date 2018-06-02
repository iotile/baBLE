#include "SetScanEnable.hpp"
#include "../../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"
#include "../../Events/CommandComplete/CommandComplete.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    SetScanEnable::SetScanEnable(Packet::Type initial_type, Packet::Type translated_type)
        : RequestPacket(initial_type, translated_type) {
      m_id = Packet::Id::ReadRequest;
      m_response_packet_code = Format::HCI::EventCode::CommandComplete;

      m_state = false;
      m_filter_duplicates = true;
    }

    void SetScanEnable::set_state(bool state) {
      m_state = state;
    }

    void SetScanEnable::unserialize(AsciiFormatExtractor& extractor) {
      try {
        m_state = AsciiFormat::string_to_number<bool>(extractor.get_string());
        m_filter_duplicates = AsciiFormat::string_to_number<bool>(extractor.get_string());

      } catch (const Exceptions::WrongFormatException& err) {
        throw Exceptions::InvalidCommandException("Invalid arguments for 'SetScanEnable' packet."
                                                  "Usage: <uuid>,<command_code>,<controller_id>,"
                                                  "<state>,<filter_duplicates>", m_uuid_request);
      }
    }

    void SetScanEnable::unserialize(FlatbuffersFormatExtractor& extractor) {
      m_state = false;
    }

    vector<uint8_t> SetScanEnable::serialize(AsciiFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      builder
          .set_name("SetScanEnable")
          .add("State", m_state)
          .add("Filter duplicates", m_filter_duplicates);

      return builder.build();
    }

    vector<uint8_t> SetScanEnable::serialize(HCIFormatBuilder& builder) const {
      RequestPacket::serialize(builder);

      builder
          .add(m_state)
          .add(m_filter_duplicates);

      return builder.build(Format::HCI::Type::Command);
    }

    vector<uint8_t> SetScanEnable::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateStopScan(builder);

      return builder.build(payload, BaBLE::Payload::StopScan);
    }

    shared_ptr<Packet::AbstractPacket> SetScanEnable::on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                               const shared_ptr<Packet::AbstractPacket>& packet) {
      LOG.debug("Response received", "SetScanEnable");
      auto response_packet = dynamic_pointer_cast<Packet::Events::CommandComplete>(packet);
      if (response_packet == nullptr) {
        throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to CommandComplete packet.");
      }

      vector<uint8_t> result = response_packet->get_returned_params();
      if (result.empty()) {
        throw Exceptions::WrongFormatException("Response received with wrong result format (SetScanEnable).", m_uuid_request);
      }

      set_status(result.at(0));

      return shared_from(this);
    }

  }

}