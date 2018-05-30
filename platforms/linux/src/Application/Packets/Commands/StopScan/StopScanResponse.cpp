#include "StopScanResponse.hpp"

using namespace std;

namespace Packet::Commands {

  StopScanResponse::StopScanResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::StopScanResponse;
    m_address_type = 0;
  }

  void StopScanResponse::unserialize(MGMTFormatExtractor& extractor) {
    ResponsePacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success) {
      m_address_type = extractor.get_value<uint8_t>();
    }
  };

  vector<uint8_t> StopScanResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("StopScan")
        .add("Address type", m_address_type);

    return builder.build();
  };

  vector<uint8_t> StopScanResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto payload = BaBLE::CreateStopScan(builder, m_address_type);

    return builder.build(payload, BaBLE::Payload::StopScan);
  }

}

