#include "ReadByGroupType.hpp"
#include "utils/string_formats.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      ReadByGroupType::ReadByGroupType(uint16_t starting_handle, uint16_t ending_handle)
          : ControllerOnlyPacket(Packet::Id::ReadByGroupTypeRequest, initial_type(), initial_packet_code(), final_packet_code()) {
        m_starting_handle = starting_handle;
        m_ending_handle = ending_handle;
        m_error = Format::HCI::AttributeErrorCode::None;

        m_length_per_service = 0;
      }

      vector<uint8_t> ReadByGroupType::serialize(HCIFormatBuilder& builder) const {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          builder
              .set_opcode(Format::HCI::AttributeCode::ErrorResponse)
              .add(static_cast<uint8_t>(initial_packet_code()))
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
        m_uuid = extractor.get_vector<uint8_t>();

        if (m_starting_handle < 0x0001 || m_starting_handle > m_ending_handle) {
          LOG.warning("Invalid handle received: starting_handle=" + to_string(m_starting_handle) + ", ending_handle=" + to_string(m_ending_handle));
          m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
          return;
        }

        try {
          uint16_t uuid_num = Utils::uuid_to_number(m_uuid);

          if (uuid_num != Format::HCI::GattUUID::PrimaryServiceDeclaration) {
            LOG.warning("'ReadByGroupTypeRequest' with not handled uuid: " + to_string(uuid_num));
            m_error = Format::HCI::AttributeErrorCode::UnsupportedGroupType;
          }
        } catch (const invalid_argument& err) {
          LOG.warning("Can't convert UUID to number: UUID too long, size=" + to_string(m_uuid.size()));
          m_error = Format::HCI::AttributeErrorCode::UnsupportedGroupType;
        }
      }

      void ReadByGroupType::set_socket(AbstractSocket* socket) {
        auto hci_socket = dynamic_cast<HCISocket*>(socket);
        if (hci_socket == nullptr) {
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't downcast socket to HCISocket packet");
        }

        if (m_error != Format::HCI::AttributeErrorCode::None) {
          return;
        }

        uint8_t total_length = 2;  // 1 byte for opcode + 1 byte for length

        for (auto& service : hci_socket->get_services()) {
          if (service.handle >= m_starting_handle && service.handle <= m_ending_handle){
            if (m_length_per_service == 0) {
              m_length_per_service = static_cast<uint8_t>(service.uuid.size() + 4);  // 2bytes for handle + 2bytes for group end handle
            }

            if (m_length_per_service != service.uuid.size() + 4) break;

            total_length += m_length_per_service;
            if (total_length > ATT_MTU) break;

            m_services.push_back(service);
          }
        }

        if (m_services.empty()) {
          m_error = Format::HCI::AttributeErrorCode::AttributeNotFound;
        }
      }

      const string ReadByGroupType::stringify() const {
        stringstream result;

        result << "<ReadByGroupType (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Starting handle: " << to_string(m_starting_handle) << ", "
               << "Ending handle: " << to_string(m_ending_handle) << ", "
               << "UUID: " << Utils::format_uuid(m_uuid) << ", ";

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
