#include "GetControllersIdsRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetControllersIdsRequest::GetControllersIdsRequest(Packet::Type initial_type, Packet::Type translated_type)
        : RequestPacket(initial_type, translated_type) {
      m_id = Packet::Id::GetControllersIdsRequest;
    }

    void GetControllersIdsRequest::unserialize(AsciiFormatExtractor& extractor) {}

    void GetControllersIdsRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetControllersIdsRequest::serialize(AsciiFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      builder.set_name("GetControllersIds");

      return builder.build();
    }

    vector<uint8_t> GetControllersIdsRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      return builder.build();
    }

  }

}