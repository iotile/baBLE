#include "ReadRequest.hpp"
#include "utils/string_formats.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      ReadRequest::ReadRequest(uint16_t attribute_handle)
          : ControllerToHostPacket(Packet::Id::ReadRequest, initial_type(), initial_packet_code(), final_packet_code()) {
        m_attribute_handle = attribute_handle;
        m_type = ReadType::None;
        m_error = Format::HCI::AttributeErrorCode::None;
      }

      // TODO: split Connection into Connection/ProbeService/ProbeCharacteristics in Python part (only in BaBLE)

      vector<uint8_t> ReadRequest::serialize(HCIFormatBuilder& builder) const {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          builder
              .set_opcode(Format::HCI::AttributeCode::ErrorResponse)
              .add(static_cast<uint8_t>(initial_packet_code()))
              .add(m_attribute_handle)
              .add(m_error);

        } else {
          builder.set_opcode(Format::HCI::AttributeCode::ReadResponse);

          switch(m_type) {
            case ReadType::Service:
              builder.add(m_service.uuid);
              break;

            case ReadType::Characteristic:
              builder
                  .add(m_characteristic.properties)
                  .add(m_characteristic.value_handle)
                  .add(m_characteristic.uuid);
              break;

            case ReadType::CharacteristicConfiguration:
              builder.add(m_characteristic.configuration);
              break;

            case ReadType::CharacteristicValue:
              builder.add(m_characteristic.const_value);

            default:
              throw Exceptions::BaBLEException(BaBLE::StatusCode::Unknown, "Unknown type to read, type=" + to_string(m_type));
          }
        }

        return builder.build(Format::HCI::Type::AsyncData);
      }

      vector<uint8_t> ReadRequest::serialize(FlatbuffersFormatBuilder& builder) const {
        auto payload = BaBLE::CreateReadPeripheral(
            builder,
            m_connection_handle,
            m_attribute_handle
        );

        return builder.build(payload, BaBLE::Payload::ReadPeripheral);
      }

      void ReadRequest::unserialize(HCIFormatExtractor& extractor) {
        m_attribute_handle = extractor.get_value<uint16_t>();

        if (m_attribute_handle < 0x0001 || m_attribute_handle > 0xFFFF) {
          LOG.warning("Invalid attribute handle received: " + to_string(m_attribute_handle), "ReadRequest");
          m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
          return;
        }
      }

      void ReadRequest::prepare(const shared_ptr<PacketRouter>& router) {
        if (m_type != ReadType::CharacteristicValue || !m_characteristic.const_value.empty() || m_error != Format::HCI::AttributeErrorCode::None) {
          m_packet_code = Format::HCI::AttributeCode::ReadResponse;
          m_final_type = Packet::Type::HCI;
        } else {
          ControllerToHostPacket::prepare(router);
        }
      }

      void ReadRequest::set_socket(AbstractSocket* socket) {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          return;
        }

        auto hci_socket = dynamic_cast<HCISocket*>(socket);
        if (hci_socket == nullptr) {
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't downcast socket to HCISocket packet");
        }

        for (auto& service : hci_socket->get_services()) {
          if (service.handle == m_attribute_handle) {
            m_type = ReadType::Service;
            m_service = service;
            break;
          }
        }

        for (auto& characteristic : hci_socket->get_characteristics()) {
          if (characteristic.handle == m_attribute_handle) {
            m_type = ReadType::Characteristic;
            m_characteristic = characteristic;
            break;
          } else if (characteristic.value_handle == m_attribute_handle) {
            m_type = ReadType::CharacteristicValue;
            m_characteristic = characteristic;
            if ((characteristic.properties & 1 << 1) == 0) {
              // Not readable
              m_error = Format::HCI::AttributeErrorCode::ReadNotPermitted;
            }
            break;
          } else if (characteristic.config_handle == m_attribute_handle) {
            m_type = ReadType::CharacteristicConfiguration;
            m_characteristic = characteristic;
            break;
          }
        }

        if (m_type == ReadType::None) {
          m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
        }
      }

      const string ReadRequest::stringify() const {
        stringstream result;

        result << "<ReadRequest (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Attribute handle: " << to_string(m_attribute_handle);

        return result.str();
      }

    }

  }

}
