#include "StartScanRequest.hpp"

using namespace std;

namespace Packet::Commands {

  StartScanRequest::StartScanRequest(Packet::Type initial_type, Packet::Type translated_type)
  : RequestPacket<StartScanRequest>(initial_type, translated_type) {
    m_id = Packet::Id::StartScanRequest;
    m_address_type = 0;
  }

  void StartScanRequest::unserialize(AsciiFormatExtractor& extractor) {
    try {
      m_address_type = AsciiFormat::string_to_number<uint8_t>(extractor.get_string());

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'StartScan' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,"
                                                       "<address_type>", m_uuid_request);
    }
  }

  void StartScanRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
    auto payload = extractor.get_payload<const BaBLE::StartScan*>();

    m_address_type = payload->address_type();
  }

  vector<uint8_t> StartScanRequest::serialize(AsciiFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder
      .set_name("StartScan")
      .add("Address type", m_address_type);

    return builder.build();
  }

  vector<uint8_t> StartScanRequest::serialize(MGMTFormatBuilder& builder) const {
    RequestPacket::serialize(builder);
    builder.add(m_address_type);

    return builder.build();
  }

}
