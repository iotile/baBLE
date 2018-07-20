#include <algorithm>
#include "utils/string_formats.hpp"
#include "FindInformation.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      FindInformation::FindInformation(uint16_t starting_handle, uint16_t ending_handle)
          : ControllerOnlyPacket(Packet::Id::FindInformation, initial_type(), initial_packet_code(), final_packet_code()) {
        m_starting_handle = starting_handle;
        m_ending_handle = ending_handle;
        m_format = 0x00;
        m_error = Format::HCI::AttributeErrorCode::None;
      }

      void FindInformation::set_gatt_table(const std::vector<Format::HCI::Service>& services, const std::vector<Format::HCI::Characteristic>& characteristics) {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          return;
        }

        for (auto& service : services) {
          if (service.handle >= m_starting_handle && service.handle <= m_ending_handle) {
            m_info[service.handle] = vector<uint8_t>{
              static_cast<uint8_t>(Format::HCI::GattUUID::PrimaryServiceDeclaration & 0x00FF),
              static_cast<uint8_t>(Format::HCI::GattUUID::PrimaryServiceDeclaration >> 8)
            };
          }
        }

        for (auto& characteristic : characteristics) {
          if (characteristic.handle >= m_starting_handle && characteristic.handle <= m_ending_handle) {
            m_info[characteristic.handle] = vector<uint8_t>{
                static_cast<uint8_t>(Format::HCI::GattUUID::CharacteristicDeclaration & 0x00FF),
                static_cast<uint8_t>(Format::HCI::GattUUID::CharacteristicDeclaration >> 8)
            };
          }

          if (characteristic.value_handle >= m_starting_handle && characteristic.value_handle <= m_ending_handle) {
            m_info[characteristic.value_handle] = characteristic.uuid;
          }

          if (characteristic.config_handle >= m_starting_handle && characteristic.config_handle <= m_ending_handle) {
            m_info[characteristic.config_handle] = vector<uint8_t>{
                static_cast<uint8_t>(Format::HCI::GattUUID::ClientCharacteristicConfiguration & 0x00FF),
                static_cast<uint8_t>(Format::HCI::GattUUID::ClientCharacteristicConfiguration >> 8)
            };
          }
        }

        if (m_info.empty()) {
          m_error = Format::HCI::AttributeErrorCode::AttributeNotFound;
          return;
        }

        uint8_t total_length = 2;  // 1 byte for opcode + 1 byte for format
        auto it = m_info.begin();
        while (it != m_info.end()) {
          if (m_format == 0x00) {
            m_format = static_cast<uint8_t>(it->second.size() == 2 ? 0x01 : 0x02);
          }

          if (m_format == 0x01) {
            if (it->second.size() != 2) break;
            total_length += 4;  // 2 bytes for handle + 2 bytes for uuid

          } else if (m_format == 0x02) {
            if (it->second.size() != 16) break;
            total_length += 18;  // 2 bytes for handle + 16 bytes for uuid
          }

          if (total_length > ATT_MTU) break;

          ++it;
        }

        m_info.erase(it, m_info.end());
      }

      vector<uint8_t> FindInformation::serialize(HCIFormatBuilder& builder) const {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          builder
              .set_opcode(Format::HCI::AttributeCode::ErrorResponse)
              .add(static_cast<uint8_t>(initial_packet_code()))
              .add(m_starting_handle)
              .add(m_error);
        } else {
          ControllerOnlyPacket::serialize(builder);

          builder.add(m_format);

          for (auto& info : m_info) {
            builder
                .add(info.first)
                .add(info.second);
          }
        }

        return builder.build(Format::HCI::Type::AsyncData);
      }

      void FindInformation::unserialize(HCIFormatExtractor& extractor) {
        m_starting_handle = extractor.get_value<uint16_t>();
        m_ending_handle = extractor.get_value<uint16_t>();

        if (m_starting_handle < 0x0001 || m_starting_handle > m_ending_handle) {
          LOG.warning("Invalid handle received: starting_handle=" + to_string(m_starting_handle) + ", ending_handle=" + to_string(m_ending_handle));
          m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
          return;
        }
      }

      const string FindInformation::stringify() const {
        stringstream result;

        result << "<FindInformation (Peripheral)> "
               << AbstractPacket::stringify() << ", ";

        for (auto it = m_info.begin(); it != m_info.end(); ++it) {
          result << "{ Handle: " << to_string(it->first) << ", "
                 << "UUID: " << Utils::format_uuid(it->second) << "} ";
          if (next(it) != m_info.end()) {
            result << ", ";
          }
        }

        return result.str();
      }

    }

  }

}
