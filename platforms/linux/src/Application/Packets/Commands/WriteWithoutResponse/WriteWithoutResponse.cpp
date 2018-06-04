#include "WriteWithoutResponse.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    WriteWithoutResponse::WriteWithoutResponse(Packet::Type initial_type, Packet::Type final_type)
        : RequestPacket(initial_type, final_type) {
      m_id = Packet::Id::WriteWithoutResponse;
    }

    void WriteWithoutResponse::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::WriteWithoutResponse*>();

      m_connection_id = payload->connection_handle();
      m_attribute_handle = payload->attribute_handle();
      auto raw_data_to_write = payload->value();
      m_data_to_write.assign(raw_data_to_write->begin(), raw_data_to_write->end());
    }

    vector<uint8_t> WriteWithoutResponse::serialize(HCIFormatBuilder& builder) const {
      RequestPacket::serialize(builder);

      builder
          .add(m_attribute_handle)
          .add(m_data_to_write);

      return builder.build(Format::HCI::Type::AsyncData);
    }

    const std::string WriteWithoutResponse::stringify() const {
      stringstream result;

      result << "<WriteWithoutResponse> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle) << ", "
             << "Data to write: " << Utils::format_bytes_array(m_data_to_write);

      return result.str();
    }

    void WriteWithoutResponse::before_sent(const std::shared_ptr<PacketRouter>& router) {
      AbstractPacket::before_sent(router);
      m_packet_code = packet_code(m_current_type);
    }

  }

}