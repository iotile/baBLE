#include "Write.hpp"

using namespace std;

namespace Packet::Commands {

  Write::Write(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {};

  void Write::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    try {
      m_connection_handle = static_cast<uint16_t>(stoi(extractor.get()));
      m_attribute_handle = static_cast<uint16_t>(stoi(extractor.get()));

      string data_str = extractor.get();
      string item;
      istringstream data_stream(data_str);
      for (uint8_t& element : m_data_to_write) {
        // TODO: fix
        if (!getline(data_stream, item, '/')) {
          throw Exceptions::InvalidCommandException("Can't parse <data> argument for 'Write' packet. Use '/' to separate bytes.", m_uuid_request);
        }
        element = static_cast<uint8_t>(stoi(item));
      }

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException("Missing arguments for 'Write' packet."
                                                "Usage: <uuid>,<command_code>,<controller_id>,<connection_handle>,<attribute_handle>,<data>", m_uuid_request);
    } catch (const bad_cast& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'Write' packet. Can't cast.", m_uuid_request);
    } catch (const std::invalid_argument& err) {
      throw Exceptions::InvalidCommandException("Invalid arguments for 'Write' packet.", m_uuid_request);
    }
  };

  void Write::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
    auto payload = extractor.get_payload<const Schemas::Write*>(Schemas::Payload::Write);

    m_connection_handle = payload->connection_handle();
    m_attribute_handle = payload->attribute_handle();
    auto raw_data_to_write = payload->value();
    m_data_to_write.assign(raw_data_to_write->begin(), raw_data_to_write->end());
  };

  void Write::unserialize(HCIFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  };

  vector<uint8_t> Write::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("Write")
        .add("Connection handle", m_connection_handle)
        .add("Attribute handle", m_attribute_handle)
        .add("Data", m_data_to_write);

    return builder.build();
  };

  vector<uint8_t> Write::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto data_written = builder.CreateVector(m_data_to_write);
    auto payload = Schemas::CreateWrite(
        builder,
        m_connection_handle,
        m_attribute_handle,
        data_written
    );

    return builder.build(payload, Schemas::Payload::Write);
  }

  vector<uint8_t> Write::serialize(HCIFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    builder
        .set_connection_handle(m_connection_handle)
        .add(m_attribute_handle)
        .add(m_data_to_write);

    return builder.build();
  };

}