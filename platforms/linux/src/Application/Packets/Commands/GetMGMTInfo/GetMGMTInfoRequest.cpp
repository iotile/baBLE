#include "GetMGMTInfoRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetMGMTInfoRequest::GetMGMTInfoRequest(Packet::Type initial_type, Packet::Type translated_type)
        : RequestPacket(initial_type, translated_type) {
      m_id = Packet::Id::GetMGMTInfoRequest;
    };

    void GetMGMTInfoRequest::unserialize(AsciiFormatExtractor& extractor) {};

    void GetMGMTInfoRequest::unserialize(FlatbuffersFormatExtractor& extractor) {};

    vector<uint8_t> GetMGMTInfoRequest::serialize(AsciiFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      builder.set_name("GetMGMTInfo");

      return builder.build();
    };

    vector<uint8_t> GetMGMTInfoRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      return builder.build();
    };

  }

}