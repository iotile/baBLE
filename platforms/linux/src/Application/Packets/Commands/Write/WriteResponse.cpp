#include "WriteResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    WriteResponse::WriteResponse(Packet::Type initial_type, Packet::Type final_type)
        : ResponsePacket(initial_type, final_type) {
      m_id = Packet::Id::WriteResponse;
    }

    void WriteResponse::unserialize(HCIFormatExtractor& extractor) {}

    vector<uint8_t> WriteResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateWrite(
          builder,
          m_connection_id,
          m_attribute_handle
      );

      return builder.build(payload, BaBLE::Payload::Write);
    }

    const std::string WriteResponse::stringify() const {
      stringstream result;

      result << "<WriteResponse> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle);

      return result.str();
    }

  }

}