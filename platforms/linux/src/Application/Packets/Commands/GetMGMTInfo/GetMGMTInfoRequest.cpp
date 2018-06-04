#include "GetMGMTInfoRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetMGMTInfoRequest::GetMGMTInfoRequest(Packet::Type initial_type, Packet::Type final_type)
        : RequestPacket(initial_type, final_type) {
      m_id = Packet::Id::GetMGMTInfoRequest;
    }

    void GetMGMTInfoRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetMGMTInfoRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      return builder.build();
    }

    const std::string GetMGMTInfoRequest::stringify() const {
      stringstream result;

      result << "<GetMGMTInfoRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}