#include "GetControllerInfoRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetControllerInfoRequest::GetControllerInfoRequest(Packet::Type initial_type, Packet::Type final_type)
        : RequestPacket(initial_type, final_type) {
      m_id = Packet::Id::GetControllerInfoRequest;
    }

    void GetControllerInfoRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetControllerInfoRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      return builder.build();
    }

    const std::string GetControllerInfoRequest::stringify() const {
      stringstream result;

      result << "<GetControllerInfoRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}