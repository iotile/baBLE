#include "WriteRequest.hpp"
#include "utils/string_formats.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      WriteRequest::WriteRequest(uint16_t attribute_handle, vector<uint8_t> value)
          : ControllerToHostPacket(Packet::Id::WriteRequest, initial_type(), initial_packet_code(), final_packet_code()) {
        m_attribute_handle = attribute_handle;
        m_value = move(value);
        m_error = Format::HCI::AttributeErrorCode::None;
      }

      vector<uint8_t> WriteRequest::serialize(HCIFormatBuilder& builder) const {
        builder
            .set_opcode(Format::HCI::AttributeCode::ErrorResponse)
            .add(static_cast<uint8_t>(initial_packet_code()))
            .add(m_attribute_handle)
            .add(m_error);

        return builder.build(Format::HCI::Type::AsyncData);
      }

      vector<uint8_t> WriteRequest::serialize(FlatbuffersFormatBuilder& builder) const {
        auto value = builder.CreateVector(m_value);

        auto payload = BaBLE::CreateWritePeripheral(
            builder,
            m_connection_handle,
            m_attribute_handle,
            value
        );

        return builder.build(payload, BaBLE::Payload::WritePeripheral);
      }

      void WriteRequest::unserialize(HCIFormatExtractor& extractor) {
        m_attribute_handle = extractor.get_value<uint16_t>();
        m_value = extractor.get_vector<uint8_t>();

        if (m_attribute_handle < 0x0001 || m_attribute_handle > 0xFFFF) {
          LOG.warning("Invalid attribute handle received: " + to_string(m_attribute_handle), "WriteRequest");
          m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
          return;
        }
      }

      void WriteRequest::prepare(const shared_ptr<PacketRouter>& router) {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          m_packet_code = Format::HCI::AttributeCode::ReadResponse;
          m_final_type = Packet::Type::HCI;
        } else {
          ControllerToHostPacket::prepare(router);
        }
      }

      void WriteRequest::set_socket(AbstractSocket* socket) {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          return;
        }

        auto hci_socket = dynamic_cast<HCISocket*>(socket);
        if (hci_socket == nullptr) {
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't downcast socket to HCISocket packet");
        }

        for (auto& characteristic : hci_socket->get_characteristics()) {
          if (characteristic.value_handle == m_attribute_handle) {
            if ((characteristic.properties & 1 << 3) == 0) {
              // Not writable
              m_error = Format::HCI::AttributeErrorCode::WriteNotPermitted;
            }
            return;
          } else if (characteristic.config_handle == m_attribute_handle) {
            return;
          }
        }

        m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
      }

      const string WriteRequest::stringify() const {
        stringstream result;

        result << "<WriteRequest (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Attribute handle: " << to_string(m_attribute_handle) << ", "
               << "Data to write: " << Utils::format_bytes_array(m_value);

        return result.str();
      }

    }

  }

}
