#include "NotificationReceived.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    NotificationReceived::NotificationReceived(Packet::Type initial_type, Packet::Type translated_type)
        : ResponsePacket(initial_type, translated_type) {
      m_id = Packet::Id::NotificationReceived;
      m_attribute_handle = 0;
    }

    void NotificationReceived::unserialize(HCIFormatExtractor& extractor) {
      m_attribute_handle = extractor.get_value<uint16_t>();

      auto value_length = extractor.get_data_length() - sizeof(m_attribute_handle);
      m_value = extractor.get_vector<uint8_t>(value_length);
    }

    vector<uint8_t> NotificationReceived::serialize(FlatbuffersFormatBuilder& builder) const {
      auto value = builder.CreateVector(m_value);

      auto payload = BaBLE::CreateNotificationReceived(
          builder,
          m_connection_id,
          m_attribute_handle,
          value
      );

      return builder.build(payload, BaBLE::Payload::NotificationReceived);
    }

    const std::string NotificationReceived::stringify() const {
      stringstream result;

      result << "<NotificationReceived> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle) << ", "
             << "Data: " << Utils::format_bytes_array(m_value);

      return result.str();
    }

  }

}