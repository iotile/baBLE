#include "GetControllerInfoRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetControllerInfoRequest::GetControllerInfoRequest()
        : HostToControllerPacket(Packet::Id::GetControllerInfoRequest, final_type(), final_packet_code()) {}

    void GetControllerInfoRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetControllerInfoRequest::serialize(MGMTFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);
      return builder.build();
    }

    const string GetControllerInfoRequest::stringify() const {
      stringstream result;

      result << "<GetControllerInfoRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}