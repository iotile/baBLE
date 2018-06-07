#include <sstream>
#include "WriteResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    WriteResponse::WriteResponse()
        : ControllerToHostPacket(Packet::Id::WriteResponse, initial_type(), initial_packet_code(), final_packet_code()) {}

    void WriteResponse::unserialize(HCIFormatExtractor& extractor) {}

    vector<uint8_t> WriteResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateWrite(
          builder,
          m_connection_id,
          m_attribute_handle
      );

      return builder.build(payload, BaBLE::Payload::Write);
    }

    const string WriteResponse::stringify() const {
      stringstream result;

      result << "<WriteResponse> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle);

      return result.str();
    }

  }

}