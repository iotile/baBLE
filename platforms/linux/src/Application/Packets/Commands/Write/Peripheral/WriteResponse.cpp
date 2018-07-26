#include "WriteResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      WriteResponse::WriteResponse()
          : HostToControllerPacket(Packet::Id::WriteResponse, final_type(), final_packet_code(), false) {
        m_attribute_handle = 0;
      }

      void WriteResponse::unserialize(FlatbuffersFormatExtractor& extractor) {
        auto payload = extractor.get_payload<const BaBLE::WritePeripheral*>();

        m_connection_handle = payload->connection_handle();
        m_attribute_handle = payload->attribute_handle();
      }

      vector<uint8_t> WriteResponse::serialize(HCIFormatBuilder& builder) const {
        HostToControllerPacket::serialize(builder);

        return builder.build(Format::HCI::Type::AsyncData);
      }

      const string WriteResponse::stringify() const {
        stringstream result;

        result << "<WriteResponse (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Attribute handle: " << to_string(m_attribute_handle);

        return result.str();
      }

    }

  }

}
