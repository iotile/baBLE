#include "ReadResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      ReadResponse::ReadResponse()
          : HostToControllerPacket(Packet::Id::ReadResponse, final_type(), final_packet_code(), false) {
        m_attribute_handle = 0;
      }

      void ReadResponse::unserialize(FlatbuffersFormatExtractor& extractor) {
        auto payload = extractor.get_payload<const BaBLE::ReadPeripheral*>();

        m_attribute_handle = payload->attribute_handle();
        m_value.assign(payload->value()->begin(), payload->value()->end());
      }

      vector<uint8_t> ReadResponse::serialize(HCIFormatBuilder& builder) const {
        HostToControllerPacket::serialize(builder);

        auto data_length = static_cast<uint8_t>(m_value.size());

        builder
            .add(data_length)
            .add(m_value);

        return builder.build(Format::HCI::Type::AsyncData);
      }

      const string ReadResponse::stringify() const {
        stringstream result;

        result << "<ReadResponse (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Attribute handle: " << to_string(m_attribute_handle) << ", "
               << "Value: " << Utils::format_bytes_array(m_value);

        return result.str();
      }

    }

  }

}
