#include "GetControllersIdsRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetControllersIdsRequest::GetControllersIdsRequest()
        : HostToControllerPacket(Packet::Id::GetControllersIdsRequest, final_type(), final_packet_code()) {}

    void GetControllersIdsRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetControllersIdsRequest::serialize(MGMTFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);
      return builder.build();
    }

    const string GetControllersIdsRequest::stringify() const {
      stringstream result;

      result << "<GetControllersIdsRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}