#include "EmitNotification.hpp"
#include "utils/string_formats.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      EmitNotification::EmitNotification(uint16_t attribute_handle, std::vector<uint8_t> value)
          : HostToControllerPacket(Packet::Id::EmitNotification, final_type(), final_packet_code(), false) {
        m_attribute_handle = attribute_handle;
        m_value = move(value);

        m_ack_to_send = false;
      }

      void EmitNotification::unserialize(FlatbuffersFormatExtractor& extractor) {
        auto payload = extractor.get_payload<const BaBLE::EmitNotification*>();

        m_connection_handle = payload->connection_handle();
        m_attribute_handle = payload->attribute_handle();
        m_value.assign(payload->value()->begin(), payload->value()->end());
      }

      vector<uint8_t> EmitNotification::serialize(HCIFormatBuilder& builder) const {
        HostToControllerPacket::serialize(builder);

        builder
            .add(m_attribute_handle)
            .add(m_value);

        return builder.build(Format::HCI::Type::AsyncData);
      }

      void EmitNotification::set_socket(AbstractSocket* socket) {
        auto hci_socket = dynamic_cast<HCISocket*>(socket);
        if (hci_socket == nullptr) {
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't downcast socket to HCISocket packet");
        }

        for (auto& characteristic : hci_socket->get_characteristics()) {
          if (characteristic.value_handle == m_attribute_handle) {
            if ((characteristic.properties & 1 << 4) == 0) {
              // Not notifiable
              throw Exceptions::BaBLEException(BaBLE::StatusCode::InvalidCommand, "Attribute can't notify (not allowed in properties)", m_uuid_request);
            }
            return;
          }
        }
      }

      vector<uint8_t> EmitNotification::serialize(FlatbuffersFormatBuilder& builder) const {
        auto payload = BaBLE::CreateEmitNotification(
            builder,
            m_connection_handle,
            m_attribute_handle
        );

        return builder.build(payload, BaBLE::Payload::EmitNotification);
      }

      void EmitNotification::prepare(const shared_ptr<PacketRouter>& router) {
        if (!m_ack_to_send) {
          m_current_type = m_final_type;
          m_ack_to_send = true;
        } else {
          m_current_type = initial_type();
        }
      }

      const string EmitNotification::stringify() const {
        stringstream result;

        result << "<EmitNotification (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Attribute handle: " << to_string(m_attribute_handle) << ", "
               << "Value: " << Utils::format_bytes_array(m_value);

        return result.str();
      }

    }

  }

}
