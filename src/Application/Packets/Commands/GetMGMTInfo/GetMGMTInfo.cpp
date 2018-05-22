#include "GetMGMTInfo.hpp"

using namespace std;

namespace Packet::Commands {

  GetMGMTInfo::GetMGMTInfo(Packet::Type initial_type, Packet::Type translated_type)
  : CommandPacket(initial_type, translated_type) {
    m_version = 0;
    m_revision = 0;
  };

  void GetMGMTInfo::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  };

  void GetMGMTInfo::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  };

  void GetMGMTInfo::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_status == Schemas::StatusCode::Success) {
      m_version = extractor.get_value<uint8_t>();
      m_revision = extractor.get_value<uint16_t>();
    }
  };

  vector<uint8_t> GetMGMTInfo::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
      .set_name("GetMGMTInfo")
      .add("Version", m_version)
      .add("Revision", m_revision);

    return builder.build();
  };

  vector<uint8_t> GetMGMTInfo::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    auto payload = Schemas::CreateGetMGMTInfo(builder, m_version, m_revision);

    return builder.build(payload, Schemas::Payload::GetMGMTInfo);
  }

  vector<uint8_t> GetMGMTInfo::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    return builder.build();
  };

}