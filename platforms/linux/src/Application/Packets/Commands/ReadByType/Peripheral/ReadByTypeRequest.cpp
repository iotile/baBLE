#include "utils/string_formats.hpp"
#include "ReadByTypeRequest.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      ReadByTypeRequest::ReadByTypeRequest()
          : ControllerToHostPacket(Packet::Id::ReadByTypeRequest, initial_type(), initial_packet_code(), final_packet_code()) {
        m_starting_handle = 0x0001;
        m_ending_handle = 0xFFFF;
        m_error = Format::HCI::AttributeErrorCode::None;

        m_read_attribute = false;
        m_uuid_num = 0;
        m_length_per_characteristic = 0;
      }

      void ReadByTypeRequest::set_characteristics(const std::vector<Format::HCI::Characteristic>& characteristics) {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          return;
        }

        if (m_read_attribute) {
          for (auto& characteristic : characteristics) {
            if (characteristic.uuid == m_uuid) {
              m_characteristics.push_back(characteristic);
              break;
            }
          }
        } else {
          uint8_t total_length = 2;  // 1 byte for opcode + 1 byte for length

          for (auto& characteristic : characteristics) {
            if (characteristic.handle >= m_starting_handle && characteristic.value_handle <= m_ending_handle
                && (characteristic.config_handle == 0 || characteristic.config_handle <= m_ending_handle)
                ) {

              if (m_uuid_num == Format::HCI::GattUUID::CharacteristicDeclaration) {
                if (m_length_per_characteristic == 0) {
                  m_length_per_characteristic = static_cast<uint8_t>(characteristic.uuid.size() + 5);  // 2 bytes for handle + 1 byte for properties + 2 bytes for value handle
                }

                if (m_length_per_characteristic != characteristic.uuid.size() + 5) break;

                total_length += m_length_per_characteristic;

              } else {
                total_length += 4;  // 2 bytes for handle + 2 bytes for configuration
              }

              if (total_length > Format::HCI::acl_mtu) break;

              m_characteristics.push_back(characteristic);
            }
          }
        }

        if (m_characteristics.empty()) {
          m_error = Format::HCI::AttributeErrorCode::AttributeNotFound;
        }
      }

      vector<uint8_t> ReadByTypeRequest::serialize(HCIFormatBuilder& builder) const {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          builder
              .set_opcode(Format::HCI::AttributeCode::ErrorResponse)
              .add(static_cast<uint8_t>(m_packet_code))
              .add(m_starting_handle)
              .add(m_error);
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
        auto payload = BaBLE::CreateReadPeripheral(
            builder,
            m_connection_handle,
            m_characteristics.at(0).value_handle
        );

        return builder.build(payload, BaBLE::Payload::ReadPeripheral);
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
        }
      }

      const string ReadByTypeRequest::stringify() const {
        stringstream result;

        result << "<ReadByTypeRequest (Peripheral)> "
               << AbstractPacket::stringify() << ", ";

        for (auto it = m_characteristics.begin(); it != m_characteristics.end(); ++it) {
          result << "{ Handle: " << to_string(it->handle) << ", "
                 << "Properties: " << to_string(it->properties) << ", "
                 << "Value handle: " << to_string(it->value_handle) << ", "
                 << "Config handle: " << to_string(it->config_handle) << ", "
                 << "Configuration: " << to_string(it->configuration) << ", "
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
