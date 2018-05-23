#include "StartScan.hpp"

using namespace std;

namespace Packet::Commands {

  StartScan::StartScan(Packet::Type initial_type, Packet::Type translated_type)
  : CommandPacket(initial_type, translated_type) {
    m_id = BaBLE::Payload::StartScan;
    m_address_type = 0;
  };

  void StartScan::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    try {
      m_address_type = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'StartScan' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<address_type>", m_uuid_request);
    }
  };

  void StartScan::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const BaBLE::StartScan*>();

    m_address_type = payload->address_type();
  };

  void StartScan::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_status == BaBLE::StatusCode::Success){
      m_address_type = extractor.get_value<uint8_t>();
    }
  };

  vector<uint8_t> StartScan::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
      .set_name("StartScan")
      .add("Address type", m_address_type);

    return builder.build();
  };

  vector<uint8_t> StartScan::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    auto payload = BaBLE::CreateStartScan(builder, m_address_type);

    return builder.build(payload, BaBLE::Payload::StartScan);
  }

  vector<uint8_t> StartScan::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder.add(m_address_type);
    return builder.build();
  };

}
