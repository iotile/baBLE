#include "GetConnectedDevicesRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetConnectedDevicesRequest::GetConnectedDevicesRequest()
        : HostToControllerPacket(Packet::Id::GetConnectedDevicesRequest, final_type(), final_packet_code()) {}

    void GetConnectedDevicesRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetConnectedDevicesRequest::serialize(MGMTFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);
      return builder.build();
    }

    const string GetConnectedDevicesRequest::stringify() const {
      stringstream result;

      result << "<GetConnectedDevicesRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}