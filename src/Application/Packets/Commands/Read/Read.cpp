#include "Read.hpp"

using namespace std;

namespace Packet::Commands {

  Read::Read(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {};

  void Read::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    try {
      m_connection_handle = static_cast<uint16_t>(stoi(extractor.get()));
      m_attribute_handle = static_cast<uint16_t>(stoi(extractor.get()));

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Missing arguments for 'Read' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<connection_handle>,<attribute_handle>", m_uuid_request);
    } catch (const bad_cast& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'Read' packet. Can't cast.", m_uuid_request);
    } catch (const std::invalid_argument& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'Read' packet.", m_uuid_request);
    }
  };

  void Read::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const Schemas::Read*>(Schemas::Payload::Read);

    m_connection_handle = payload->connection_handle();
    m_attribute_handle = payload->attribute_handle();
  };

  void Read::unserialize(HCIFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    uint16_t data_length = extractor.get_data_length();
    m_data_read = extractor.get_vector<uint8_t>(data_length);
  };

  vector<uint8_t> Read::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("Read")
        .add("Connection handle", m_connection_handle)
        .add("Attribute handle", m_attribute_handle)
        .add("Data", m_data_read);

    return builder.build();
  };

  vector<uint8_t> Read::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto data_read = builder.CreateVector(m_data_read);
    auto payload = Schemas::CreateRead(
        builder,
        m_connection_handle,
        m_attribute_handle,
        data_read
    );

    return builder.build(payload, Schemas::Payload::Read);
  }

  vector<uint8_t> Read::serialize(HCIFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    builder
        .set_connection_handle(m_connection_handle)
        .set_attribute_handle(m_attribute_handle);

    return builder.build();
  };

}