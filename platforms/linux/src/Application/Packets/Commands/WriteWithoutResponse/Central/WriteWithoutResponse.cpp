#include "WriteWithoutResponse.hpp"
#include "utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Central {

      WriteWithoutResponse::WriteWithoutResponse(uint16_t attribute_handle, vector<uint8_t> data)
          : HostToControllerPacket(Packet::Id::WriteWithoutResponse, final_type(), final_packet_code(), false) {
        m_attribute_handle = attribute_handle;
        m_data_to_write = move(data);

        m_ack_to_send = false;
      }

      void WriteWithoutResponse::unserialize(FlatbuffersFormatExtractor& extractor) {
        auto payload = extractor.get_payload<const BaBLE::WriteWithoutResponseCentral*>();

        m_connection_handle = payload->connection_handle();
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

      vector<uint8_t> WriteWithoutResponse::serialize(FlatbuffersFormatBuilder& builder) const {
        auto payload = BaBLE::CreateWriteWithoutResponseCentral(
            builder,
            m_connection_handle,
            m_attribute_handle
        );

        return builder.build(payload, BaBLE::Payload::WriteWithoutResponseCentral);
      }

      void WriteWithoutResponse::prepare(const shared_ptr<PacketRouter>& router) {
        if (!m_ack_to_send) {
          m_current_type = m_final_type;
          m_ack_to_send = true;
        } else {
          m_current_type = initial_type();
        }
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

}