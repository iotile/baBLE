#include "GetMGMTInfoResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetMGMTInfoResponse::GetMGMTInfoResponse(Packet::Type initial_type, Packet::Type final_type)
        : ResponsePacket(initial_type, final_type) {
      m_id = Packet::Id::GetMGMTInfoResponse;
      m_version = 0;
      m_revision = 0;
    }

    void GetMGMTInfoResponse::unserialize(MGMTFormatExtractor& extractor) {
      ResponsePacket::unserialize(extractor);

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