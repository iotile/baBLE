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

    void SetScanEnable::unserialize(FlatbuffersFormatExtractor& extractor) {
      m_state = false;
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

    const std::string SetScanEnable::stringify() const {
      stringstream result;

      result << "<SetScanEnable> "
             << AbstractPacket::stringify() << ", "
             << "State: " << to_string(m_state) << ", "
             << "Filter duplicates: " << to_string(m_filter_duplicates);

      return result.str();
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