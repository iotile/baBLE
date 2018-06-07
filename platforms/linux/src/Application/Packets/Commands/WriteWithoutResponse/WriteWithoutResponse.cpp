#include "WriteWithoutResponse.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    WriteWithoutResponse::WriteWithoutResponse(uint16_t attribute_handle, vector<uint8_t> data)
        : HostToControllerPacket(Packet::Id::WriteWithoutResponse, final_type(), final_packet_code(), false) {
      m_attribute_handle = attribute_handle;
      m_data_to_write = move(data);
    }

    void WriteWithoutResponse::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::WriteWithoutResponse*>();

      m_connection_id = payload->connection_handle();
      m_attribute_handle = payload->attribute_handle();
      auto raw_data_to_write = payload->value();
      m_data_to_write.assign(raw_data_to_write->begin(), raw_data_to_write->end());
    }

    vector<uint8_t> WriteWithoutResponse::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_attribute_handle)
          .add(m_data_to_write);

      return builder.build(Format::HCI::Type::AsyncData);
    }

    const string WriteWithoutResponse::stringify() const {
      stringstream result;

      result << "<WriteWithoutResponse> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle) << ", "
             << "Data to write: " << Utils::format_bytes_array(m_data_to_write);

      return result.str();
    }

  }

}