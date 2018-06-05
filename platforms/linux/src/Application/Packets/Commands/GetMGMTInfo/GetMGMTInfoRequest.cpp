#include "GetMGMTInfoRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetMGMTInfoRequest::GetMGMTInfoRequest()
        : HostToControllerPacket(Packet::Id::GetMGMTInfoRequest, final_type(), final_packet_code()) {}

    void GetMGMTInfoRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetMGMTInfoRequest::serialize(MGMTFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);
      return builder.build();
    }

    const string GetMGMTInfoRequest::stringify() const {
      stringstream result;

      result << "<GetMGMTInfoRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}