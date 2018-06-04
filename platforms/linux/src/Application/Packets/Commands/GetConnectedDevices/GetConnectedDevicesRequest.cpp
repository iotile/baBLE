#include "GetConnectedDevicesRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    GetConnectedDevicesRequest::GetConnectedDevicesRequest(Packet::Type initial_type, Packet::Type translated_type)
        : RequestPacket(initial_type, translated_type) {
      m_id = Packet::Id::GetConnectedDevicesRequest;
    }

    void GetConnectedDevicesRequest::unserialize(FlatbuffersFormatExtractor& extractor) {}

    vector<uint8_t> GetConnectedDevicesRequest::serialize(MGMTFormatBuilder& builder) const {
      RequestPacket::serialize(builder);
      return builder.build();
    }

    const std::string GetConnectedDevicesRequest::stringify() const {
      stringstream result;

      result << "<GetConnectedDevicesRequest> "
             << AbstractPacket::stringify();

      return result.str();
    }

  }

}