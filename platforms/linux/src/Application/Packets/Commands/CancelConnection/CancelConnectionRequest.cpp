#include "CancelConnectionRequest.hpp"
#include "CancelConnectionResponse.hpp"
#include "../../Events/CommandComplete/CommandComplete.hpp"
#include "../../../../Exceptions/BaBLEException.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    CancelConnectionRequest::CancelConnectionRequest()
        : HostToControllerPacket(Packet::Id::CancelConnectionRequest, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::EventCode::CommandComplete;
    }

    void CancelConnectionRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> CancelConnectionRequest::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      return builder.build(Format::HCI::Type::Command);
    }

    const std::string CancelConnectionRequest::stringify() const {
      stringstream result;

      result << "<CancelConnectionRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

    shared_ptr<Packet::AbstractPacket> CancelConnectionRequest::on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                           const shared_ptr<Packet::AbstractPacket>& packet) {
      LOG.debug("Response received", "CancelConnectionRequest");
      auto command_complete_packet = dynamic_pointer_cast<Packet::Events::CommandComplete>(packet);
      if (command_complete_packet == nullptr) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't downcast AbstractPacket to CommandComplete packet (CancelConnectionRequest).",
            m_uuid_request
        );
      }

      vector<uint8_t> result = command_complete_packet->get_returned_params();
      if (result.empty()) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::WrongFormat,
            "Response received with no result (CancelConnectionRequest).",
            m_uuid_request
        );
      }

      shared_ptr<Packet::Commands::CancelConnectionResponse> response_packet = make_shared<Packet::Commands::CancelConnectionResponse>();
      response_packet->set_status(result.at(0), true, "HCI");
      response_packet->set_uuid_request(m_uuid_request);
      response_packet->set_controller_id(m_controller_id);

      return response_packet;
    }

  }

}
