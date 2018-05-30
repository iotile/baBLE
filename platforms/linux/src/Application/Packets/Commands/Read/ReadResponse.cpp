#include "ReadResponse.hpp"

using namespace std;

namespace Packet::Commands {

  ReadResponse::ReadResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::ReadResponse;
    m_attribute_handle = 0;
  };

  void ReadResponse::unserialize(HCIFormatExtractor& extractor) {
    uint16_t data_length = extractor.get_data_length();
    m_data_read = extractor.get_vector<uint8_t>(data_length);
  };

  vector<uint8_t> ReadResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("Read")
        .add("Attribute handle", m_attribute_handle)
        .add("Data", m_data_read);

    return builder.build();
  };

  vector<uint8_t> ReadResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto data_read = builder.CreateVector(m_data_read);
    auto payload = BaBLE::CreateRead(
        builder,
        m_connection_id,
        m_attribute_handle,
        data_read
    );

    return builder.build(payload, BaBLE::Payload::Read);
  }

}