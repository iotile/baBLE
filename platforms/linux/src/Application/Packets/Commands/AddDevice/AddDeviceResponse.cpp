#include "AddDeviceResponse.hpp"

using namespace std;

namespace Packet::Commands {

  AddDeviceResponse::AddDeviceResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::AddDeviceResponse;
    m_address_type = 0;
  }

  void AddDeviceResponse::unserialize(MGMTFormatExtractor& extractor) {
    ResponsePacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success){
      m_address = extractor.get_array<uint8_t, 6>();
      m_address_type = extractor.get_value<uint8_t>();
    }
  }

  vector<uint8_t> AddDeviceResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("AddDevice")
        .add("Address type", m_address_type)
        .add("Address", m_address);

    return builder.build();
  }

  vector<uint8_t> AddDeviceResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto address = builder.CreateVector(m_address.data(), m_address.size());

    auto payload = BaBLE::CreateAddDevice(builder, address, m_address_type);

    return builder.build(payload, BaBLE::Payload::AddDevice);
  }

}
