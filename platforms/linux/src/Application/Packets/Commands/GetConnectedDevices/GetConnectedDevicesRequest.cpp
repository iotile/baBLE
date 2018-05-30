#include "GetConnectedDevicesRequest.hpp"

using namespace std;

namespace Packet::Commands {

  GetConnectedDevicesRequest::GetConnectedDevicesRequest(Packet::Type initial_type, Packet::Type translated_type)
      : RequestPacket(initial_type, translated_type) {
    m_id = Packet::Id::GetConnectedDevicesRequest;
  }

  void GetConnectedDevicesRequest::unserialize(AsciiFormatExtractor& extractor) {}

  void GetConnectedDevicesRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

  vector<uint8_t> GetConnectedDevicesRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder.set_name("GetConnectedDevices");

    return builder.build();
  }

  vector<uint8_t> GetConnectedDevicesRequest::serialize(MGMTFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    return builder.build();
  }

}