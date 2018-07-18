#include "ReadByTypeResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      ReadByTypeResponse::ReadByTypeResponse()
          : HostToControllerPacket(Packet::Id::ReadByTypeResponse, final_type(), final_packet_code(), false) {
        m_value_handle = 0;
      }

      void ReadByTypeResponse::unserialize(FlatbuffersFormatExtractor& extractor) {
        auto payload = extractor.get_payload<const BaBLE::ReadPeripheral*>();

        m_value_handle = payload->attribute_handle();
        m_value.assign(payload->value()->begin(), payload->value()->end());
      }

      vector<uint8_t> ReadByTypeResponse::serialize(HCIFormatBuilder& builder) const {
        HostToControllerPacket::serialize(builder);

        auto data_length = static_cast<uint8_t>(m_value.size());

        builder
            .add(data_length)
            .add(m_value_handle)
            .add(m_value);

        return builder.build(Format::HCI::Type::AsyncData);
      }

      const string ReadByTypeResponse::stringify() const {
        stringstream result;

        result << "<ReadByTypeResponse (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Value handle: " << to_string(m_value_handle) << ", "
               << "Value: " << Utils::format_bytes_array(m_value);

        return result.str();
      }

    }

  }

}