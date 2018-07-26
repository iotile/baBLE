#include "ReadByTypeRequest.hpp"
#include "utils/string_formats.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"
#include "Application/Packets/Commands/Read/Peripheral/ReadResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      ReadByTypeRequest::ReadByTypeRequest(uint16_t starting_handle, uint16_t ending_handle)
          : ControllerToHostPacket(Packet::Id::ReadByTypeRequest, initial_type(), initial_packet_code(), final_packet_code()) {
        m_starting_handle = starting_handle;
        m_ending_handle = ending_handle;
        m_error = Format::HCI::AttributeErrorCode::None;

        m_read_attribute = false;
        m_attribute_handle = 0;
        m_uuid_num = 0;
        m_length_per_characteristic = 0;
      }

      vector<uint8_t> ReadByTypeRequest::serialize(HCIFormatBuilder& builder) const {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          builder
              .set_opcode(Format::HCI::AttributeCode::ErrorResponse)
              .add(static_cast<uint8_t>(initial_packet_code()))
              .add(m_starting_handle)
              .add(m_error);
        } else if (m_attribute_handle != 0) {
          auto data_length = static_cast<uint8_t>(m_value_read.size() + 2);

          builder
              .set_opcode(Format::HCI::AttributeCode::ReadByTypeResponse)
              .add(data_length)
              .add(m_attribute_handle)
              .add(m_value_read);

        } else {
          builder
              .set_opcode(Format::HCI::AttributeCode::ReadByTypeResponse)
              .add(m_length_per_characteristic);

          if (m_uuid_num == Format::HCI::GattUUID::CharacteristicDeclaration) {
            for (auto& characteristic : m_characteristics) {
              builder
                  .add(characteristic.handle)
                  .add(characteristic.properties)
                  .add(characteristic.value_handle)
                  .add(characteristic.uuid);
            }
          } else {
            for (auto& characteristic : m_characteristics) {
              builder
                  .add(characteristic.config_handle)
                  .add(characteristic.configuration);
            }
          }
        }

        return builder.build(Format::HCI::Type::AsyncData);
      }

      vector<uint8_t> ReadByTypeRequest::serialize(FlatbuffersFormatBuilder& builder) const {
        if (m_read_packet_request == nullptr) {
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Trying to serialize a not initialized ReadRequest packet", m_uuid_request);
        }
        return m_read_packet_request->serialize(builder);
      }

      void ReadByTypeRequest::unserialize(HCIFormatExtractor& extractor) {
        m_starting_handle = extractor.get_value<uint16_t>();
        m_ending_handle = extractor.get_value<uint16_t>();
        m_uuid = extractor.get_vector<uint8_t>();

        if (m_starting_handle < 0x0001 || m_starting_handle > m_ending_handle) {
          LOG.warning("Invalid handle received: starting_handle=" + to_string(m_starting_handle) + ", ending_handle=" + to_string(m_ending_handle));
          m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
          return;
        }

        try {
          m_uuid_num = Utils::uuid_to_number(m_uuid);

          if (m_uuid_num != Format::HCI::GattUUID::CharacteristicDeclaration && m_uuid_num != Format::HCI::GattUUID::ClientCharacteristicConfiguration) {
            m_read_attribute = true;
          }
        } catch (const invalid_argument& err) {
          m_read_attribute = true;
        }
      }

      void ReadByTypeRequest::prepare(const shared_ptr<PacketRouter>& router) {
        if (!m_read_attribute || m_error != Format::HCI::AttributeErrorCode::None) {
          m_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;
          m_final_type = Packet::Type::HCI;
        } else {
          ControllerToHostPacket::prepare(router);

          m_read_packet_request = make_unique<Commands::Peripheral::ReadRequest>(m_characteristics.at(0).value_handle);
          m_read_packet_request->set_controller_id(m_controller_id);
          m_read_packet_request->set_connection_handle(m_connection_handle);

          PacketUuid response_uuid = m_read_packet_request->get_uuid();
          response_uuid.response_packet_code = Format::HCI::AttributeCode::ReadResponse;
          auto response_callback =
              [this](const shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
                return on_read_response_received(router, packet);
              };
          router->add_callback(response_uuid, shared_from(this), response_callback);
        }
      }

      shared_ptr<AbstractPacket> ReadByTypeRequest::on_read_response_received(const shared_ptr<PacketRouter>& router,
                                                                              const shared_ptr<AbstractPacket>& packet) {
        LOG.debug("Read response received", "ReadByTypeRequest");

        auto read_response_packet = dynamic_pointer_cast<Commands::Peripheral::ReadResponse>(packet);
        if (read_response_packet == nullptr) {
          throw Exceptions::BaBLEException(
              BaBLE::StatusCode::Failed,
              "Can't downcast AbstractPacket to Peripheral::ReadResponse packet (Peripheral::ReadByTypeRequest).",
              m_uuid_request
          );
        }

        m_attribute_handle = read_response_packet->get_attribute_handle();
        m_value_read = read_response_packet->get_value();
        m_read_attribute = false;

        return shared_from(this);
      }

      void ReadByTypeRequest::set_socket(AbstractSocket* socket) {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          return;
        }

        auto hci_socket = dynamic_cast<HCISocket*>(socket);
        if (hci_socket == nullptr) {
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't downcast socket to HCISocket packet");
        }

        if (m_read_attribute) {
          for (auto& characteristic : hci_socket->get_characteristics()) {
            if (characteristic.uuid == m_uuid) {
              if (m_uuid_num == Format::HCI::GattUUID::ClientCharacteristicConfiguration) {
                if (characteristic.config_handle != 0) {
                  m_attribute_handle = characteristic.config_handle;
                  m_value_read.push_back(static_cast<uint8_t>(characteristic.configuration & 0x00FF));
                  m_value_read.push_back(static_cast<uint8_t>(characteristic.configuration >> 8));
                  m_read_attribute = false;
                } else {
                  m_error = Format::HCI::AttributeErrorCode::AttributeNotFound;
                }
              } else {
                if (!characteristic.const_value.empty()) {
                  m_attribute_handle = characteristic.value_handle;
                  m_value_read = characteristic.const_value;
                  m_read_attribute = false;
                } else {
                  m_characteristics.push_back(characteristic);
                }
              }
              break;
            }
          }
        } else {
          uint8_t total_length = 2;  // 1 byte for opcode + 1 byte for length

          for (auto& characteristic : hci_socket->get_characteristics()) {
            if (characteristic.handle >= m_starting_handle && characteristic.handle <= m_ending_handle) {

              if (m_uuid_num == Format::HCI::GattUUID::CharacteristicDeclaration) {
                if (m_length_per_characteristic == 0) {
                  // 2 bytes for handle + 1 byte for properties + 2 bytes for value handle
                  m_length_per_characteristic = static_cast<uint8_t>(characteristic.uuid.size() + 5);
                }

                if (m_length_per_characteristic != characteristic.uuid.size() + 5) break;

                total_length += m_length_per_characteristic;

              } else {
                total_length += 4;  // 2 bytes for handle + 2 bytes for configuration
              }

              if (total_length > ATT_MTU) break;

              m_characteristics.push_back(characteristic);
            }
          }
        }

        if (m_characteristics.empty()) {
          m_error = Format::HCI::AttributeErrorCode::AttributeNotFound;
        }
      }

      const string ReadByTypeRequest::stringify() const {
        stringstream result;

        result << "<ReadByTypeRequest (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Starting handle: " << to_string(m_starting_handle) << ", "
               << "Ending handle: " << to_string(m_ending_handle) << ", "
               << "UUID: " << Utils::format_uuid(m_uuid) << ", ";

        for (auto it = m_characteristics.begin(); it != m_characteristics.end(); ++it) {
          result << "{ Handle: " << to_string(it->handle) << ", "
                 << "Properties: " << to_string(it->properties) << ", "
                 << "Value handle: " << to_string(it->value_handle) << ", "
                 << "Config handle: " << to_string(it->config_handle) << ", "
                 << "Configuration: " << to_string(it->configuration) << ", "
                 << "Constant value: " << Utils::format_bytes_array(it->const_value) << ", "
                 << "UUID: " << Utils::format_uuid(it->uuid) << "} ";
          if (next(it) != m_characteristics.end()) {
            result << ", ";
          }
        }

        return result.str();
      }

    }

  }

}
