#include "CancelConnectionResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    CancelConnectionResponse::CancelConnectionResponse()
        : ControllerToHostPacket(Packet::Id::CancelConnectionResponse, initial_type(), initial_packet_code(), final_packet_code()) {
    }

    vector<uint8_t> CancelConnectionResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateCancelConnection(builder);

      return builder.build(payload, BaBLE::Payload::CancelConnection);
    }

    const std::string CancelConnectionResponse::stringify() const {
      stringstream result;

      result << "<CancelConnectionResponse> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}