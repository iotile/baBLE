#include "GetControllerInfoRequest.hpp"

using namespace std;

namespace Packet::Commands {

  GetControllerInfoRequest::GetControllerInfoRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::GetControllerInfoRequest;
  };

  void GetControllerInfoRequest::unserialize(AsciiFormatExtractor& extractor) {};

  void GetControllerInfoRequest::unserialize(FlatbuffersFormatExtractor& extractor) {};

  vector<uint8_t> GetControllerInfoRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder.set_name("GetControllerInfo");

    return builder.build();
  };

  vector<uint8_t> GetControllerInfoRequest::serialize(MGMTFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    return builder.build();
  };

}