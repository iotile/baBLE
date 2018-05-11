#include "GetControllersList.hpp"

using namespace std;

namespace Packet::Commands {

  GetControllersList::GetControllersList(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_num_controllers = 0;
  };

  void GetControllersList::import(AsciiFormatExtractor& extractor) {
    CommandPacket::import(extractor);
  };

  void GetControllersList::import(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::import(extractor);
  };

  void GetControllersList::import(MGMTFormatExtractor& extractor) {
    CommandPacket::import(extractor);

    if (m_native_status == 0) {
      m_num_controllers = extractor.get_value<uint16_t>();
      m_controllers = extractor.get_vector<uint16_t>(m_num_controllers);
    }
  };

  vector<uint8_t> GetControllersList::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("GetControllersList")
        .add("Num. of controllers", m_num_controllers)
        .add("Controllers ID", m_controllers);

    return builder.build();
  };

  vector<uint8_t> GetControllersList::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto controllers = builder.CreateVector(m_controllers);
    auto payload = Schemas::CreateGetControllersList(builder, m_num_controllers, controllers);

    return builder.build(payload, Schemas::Payload::GetControllersList, m_native_class, m_status, m_native_status);
  }

  vector<uint8_t> GetControllersList::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    return builder.build();
  };

}