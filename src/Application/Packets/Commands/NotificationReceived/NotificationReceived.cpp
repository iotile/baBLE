#include "NotificationReceived.hpp"

using namespace std;

namespace Packet::Commands {

  NotificationReceived::NotificationReceived(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {
    m_id = BaBLE::Payload::NotificationReceived;
    m_connection_handle = 0;
    m_attribute_handle = 0;
  }

  void NotificationReceived::unserialize(HCIFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    m_connection_handle = extractor.get_connection_id();
    m_attribute_handle = extractor.get_value<uint16_t>();

    auto value_length = extractor.get_data_length() - sizeof(m_attribute_handle);
    m_value = extractor.get_vector<uint8_t>(value_length);
  }

  vector<uint8_t> NotificationReceived::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    builder
        .set_name("NotificationReceived")
        .add("Connection handle", m_connection_handle)
        .add("Attribute handle", m_attribute_handle)
        .add("Data", m_value);

    return builder.build();
  };

  vector<uint8_t> NotificationReceived::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    auto value = builder.CreateVector(m_value);

    auto payload = BaBLE::CreateNotificationReceived(
        builder,
        m_connection_handle,
        m_attribute_handle,
        value
    );

    return builder.build(payload, BaBLE::Payload::NotificationReceived);
  }

}