#include <algorithm>
#include "utils/string_formats.hpp"
#include "ReadByGroupType.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      ReadByGroupType::ReadByGroupType()
          : ControllerOnlyPacket(Packet::Id::ReadByGroupTypeRequest, initial_type(), initial_packet_code(), final_packet_code()) {
        m_starting_handle = 0x0001;
        m_ending_handle = 0xFFFF;
        m_uuid = 0;
        m_error = Format::HCI::AttributeErrorCode::None;

        m_length_per_service = 0;
      }

      void ReadByGroupType::set_services(const std::vector<Format::HCI::Service>& services) {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          return;
        }

        uint8_t total_length = 2;  // 1 byte for opcode + 1 byte for length

        for (auto& service : services) {
          if (service.handle >= m_starting_handle && service.group_end_handle <= m_ending_handle){
            if (m_length_per_service == 0) {
              m_length_per_service = static_cast<uint8_t>(service.uuid.size() + 4);  // 2bytes for handle + 2bytes for group end handle
            }

            if (m_length_per_service != service.uuid.size() + 4) break;

            total_length += m_length_per_service;
            if (total_length > Format::HCI::acl_mtu) break;

            m_services.push_back(service);
          }
        }

        if (m_services.empty()) {
          m_error = Format::HCI::AttributeErrorCode::AttributeNotFound;
        }
      }

      vector<uint8_t> ReadByGroupType::serialize(HCIFormatBuilder& builder) const {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          builder
              .set_opcode(Format::HCI::AttributeCode::ErrorResponse)
              .add(static_cast<uint8_t>(m_packet_code))
              .add(m_starting_handle)
              .add(m_error);
        } else {
          ControllerOnlyPacket::serialize(builder);

          builder.add(m_length_per_service);

          for (auto& service : m_services) {
            builder
                .add(service.handle)
                .add(service.group_end_handle)
                .add(service.uuid);
          }
        }

        return builder.build(Format::HCI::Type::AsyncData);
      }

      void ReadByGroupType::unserialize(HCIFormatExtractor& extractor) {
        m_starting_handle = extractor.get_value<uint16_t>();
        m_ending_handle = extractor.get_value<uint16_t>();
        m_uuid = extractor.get_value<uint16_t>();

        if (m_starting_handle < 0x0001 || m_starting_handle > m_ending_handle) {
          LOG.warning("Invalid handle received: starting_handle=" + to_string(m_starting_handle) + ", ending_handle=" + to_string(m_ending_handle));
          m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
          return;
        }

        if (m_uuid != Format::HCI::GattUUID::PrimaryServiceDeclaration) {
          LOG.warning("'ReadByTypeRequest' with not handled uuid: " + to_string(m_uuid));
          m_error = Format::HCI::AttributeErrorCode::UnsupportedGroupType;
          return;
        }
      }

      const string ReadByGroupType::stringify() const {
        stringstream result;

        result << "<ReadByGroupType> "
               << AbstractPacket::stringify() << ", ";

        for (auto it = m_services.begin(); it != m_services.end(); ++it) {
          result << "{ Handle: " << to_string(it->handle) << ", "
                 << "Group end handle: " << to_string(it->group_end_handle) << ", "
                 << "UUID: " << Utils::format_uuid(it->uuid) << "} ";
          if (next(it) != m_services.end()) {
            result << ", ";
          }
        }

        return result.str();
      }

    }

  }

}
