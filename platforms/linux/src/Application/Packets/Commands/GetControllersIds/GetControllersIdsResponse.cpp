#include "GetControllersIdsResponse.hpp"

using namespace std;

namespace Packet::Commands {

  GetControllersIdsResponse::GetControllersIdsResponse(Packet::Type initial_type, Packet::Type translated_type)
      : ResponsePacket(initial_type, translated_type) {
    m_id = Packet::Id::GetControllersIdsResponse;
  }

  void GetControllersIdsResponse::unserialize(MGMTFormatExtractor& extractor) {
    ResponsePacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success) {
      auto m_num_controllers = extractor.get_value<uint16_t>();
      m_controllers_ids = extractor.get_vector<uint16_t>(m_num_controllers);
    }
  }

  vector<uint8_t> GetControllersIdsResponse::serialize(AsciiFormatBuilder& builder) const {
    ResponsePacket::serialize(builder);
    builder
        .set_name("GetControllersIds")
        .add("Num. of controllers", m_controllers_ids.size())
        .add("Controllers ID", m_controllers_ids);

    return builder.build();
  }

  vector<uint8_t> GetControllersIdsResponse::serialize(FlatbuffersFormatBuilder& builder) const {
    auto controllers_ids = builder.CreateVector(m_controllers_ids);
    auto payload = BaBLE::CreateGetControllersIds(builder, controllers_ids);

    return builder.build(payload, BaBLE::Payload::GetControllersIds);
  }

}