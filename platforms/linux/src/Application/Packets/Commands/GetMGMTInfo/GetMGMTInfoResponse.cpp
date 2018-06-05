#include <sstream>
#include "GetMGMTInfoResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetMGMTInfoResponse::GetMGMTInfoResponse()
        : ControllerToHostPacket(Packet::Id::GetMGMTInfoResponse, initial_type(), initial_packet_code(), final_packet_code()) {
      m_version = 0;
      m_revision = 0;
    }

    void GetMGMTInfoResponse::unserialize(MGMTFormatExtractor& extractor) {
      set_status(extractor.get_value<uint8_t>());

      if (m_status == BaBLE::StatusCode::Success) {
        m_version = extractor.get_value<uint8_t>();
        m_revision = extractor.get_value<uint16_t>();
      }
    }

    vector<uint8_t> GetMGMTInfoResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateGetMGMTInfo(builder, m_version, m_revision);

      return builder.build(payload, BaBLE::Payload::GetMGMTInfo);
    }

    const std::string GetMGMTInfoResponse::stringify() const {
      stringstream result;

      result << "<GetMGMTInfoResponse> "
             << AbstractPacket::stringify() << ", "
             << "Version: " << to_string(m_version) << "." << to_string(m_revision);

      return result.str();
    }

  }

}