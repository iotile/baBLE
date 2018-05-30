#include "GetMGMTInfoResponse.hpp"

using namespace std;

namespace Packet::Commands {

  GetMGMTInfoResponse::GetMGMTInfoResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::GetMGMTInfoResponse;
    m_version = 0;
    m_revision = 0;
  };

  void GetMGMTInfoResponse::unserialize(MGMTFormatExtractor& extractor) {
    ResponsePacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success) {
      m_version = extractor.get_value<uint8_t>();
      m_revision = extractor.get_value<uint16_t>();
    }
  };

  vector<uint8_t> GetMGMTInfoResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("GetMGMTInfo")
        .add("Version", m_version)
        .add("Revision", m_revision);

    return builder.build();
  };

  vector<uint8_t> GetMGMTInfoResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto payload = BaBLE::CreateGetMGMTInfo(builder, m_version, m_revision);

    return builder.build(payload, BaBLE::Payload::GetMGMTInfo);
  }

}