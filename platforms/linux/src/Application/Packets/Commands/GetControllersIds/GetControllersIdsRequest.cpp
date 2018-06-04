#include "GetControllersIdsRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetControllersIdsRequest::GetControllersIdsRequest(Packet::Type initial_type, Packet::Type final_type)
        : RequestPacket(initial_type, final_type) {
      m_id = Packet::Id::GetControllersIdsRequest;
    }

    void GetControllersIdsRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetControllersIdsRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      return builder.build();
    }

    const std::string GetControllersIdsRequest::stringify() const {
      stringstream result;

      result << "<GetControllersIdsRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}