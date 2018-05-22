#include "GetControllersIds.hpp"

using namespace std;

namespace Packet::Commands {

  GetControllersIds::GetControllersIds(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {}

  void GetControllersIds::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  }

  void GetControllersIds::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  }

  void GetControllersIds::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_status == Schemas::StatusCode::Success) {
      auto m_num_controllers = extractor.get_value<uint16_t>();
      m_controllers_ids = extractor.get_vector<uint16_t>(m_num_controllers);
    }
  }

  vector<uint8_t> GetControllersIds::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("GetControllersList")
        .add("Num. of controllers", m_controllers_ids.size())
        .add("Controllers ID", m_controllers_ids);

    return builder.build();
  }

  vector<uint8_t> GetControllersIds::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto controllers_ids = builder.CreateVector(m_controllers_ids);
    auto payload = Schemas::CreateGetControllersIds(builder, controllers_ids);

    return builder.build(payload, Schemas::Payload::GetControllersIds);
  }

  vector<uint8_t> GetControllersIds::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    return builder.build();
  }

}