#include "WriteResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    WriteResponse::WriteResponse(Packet::Type initial_type, Packet::Type translated_type)
        : ResponsePacket(initial_type, translated_type) {
      m_id = Packet::Id::WriteResponse;
    }

    void WriteResponse::unserialize(HCIFormatExtractor& extractor) {}

    vector<uint8_t> WriteResponse::serialize(AsciiFormatBuilder& builder) const {
      ResponsePacket::serialize(builder);
      builder
          .set_name("Write")
          .add("Attribute handle", m_attribute_handle);

      return builder.build();
    };

    vector<uint8_t> WriteResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateWrite(
          builder,
          m_connection_id,
          m_attribute_handle
      );

      return builder.build(payload, BaBLE::Payload::Write);
    }

  }

}