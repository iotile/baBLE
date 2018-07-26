#include "FindByType.hpp"
#include "utils/string_formats.hpp"
#include "Transport/Socket/HCI/HCISocket.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      FindByType::FindByType(uint16_t starting_handle, uint16_t ending_handle)
          : ControllerOnlyPacket(Packet::Id::FindByType, initial_type(), initial_packet_code(), final_packet_code()) {
        m_starting_handle = starting_handle;
        m_ending_handle = ending_handle;
        m_uuid_num = Format::HCI::GattUUID::PrimaryServiceDeclaration;
        m_error = Format::HCI::AttributeErrorCode::None;
      }

      vector<uint8_t> FindByType::serialize(HCIFormatBuilder& builder) const {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          builder
              .set_opcode(Format::HCI::AttributeCode::ErrorResponse)
              .add(static_cast<uint8_t>(initial_packet_code()))
              .add(m_starting_handle)
              .add(m_error);
        } else {
          ControllerOnlyPacket::serialize(builder);

          for (auto& service : m_services) {
            builder
                .add(service.handle)
                .add(service.group_end_handle);
          }
        }

        return builder.build(Format::HCI::Type::AsyncData);
      }

      void FindByType::unserialize(HCIFormatExtractor& extractor) {
        m_starting_handle = extractor.get_value<uint16_t>();
        m_ending_handle = extractor.get_value<uint16_t>();
        m_uuid_num = extractor.get_value<uint16_t>();
        m_value = extractor.get_vector<uint8_t>();

        if (m_starting_handle < 0x0001 || m_starting_handle > m_ending_handle) {
          LOG.warning("Invalid handle received: starting_handle=" + to_string(m_starting_handle) + ", ending_handle=" + to_string(m_ending_handle));
          m_error = Format::HCI::AttributeErrorCode::InvalidHandle;
          return;
        }

        if (m_uuid_num != Format::HCI::GattUUID::PrimaryServiceDeclaration) {
          LOG.warning("'FindByTypeRequest' with not handled uuid: " + to_string(m_uuid_num));
          m_error = Format::HCI::AttributeErrorCode::UnsupportedGroupType;
        }
      }

      void FindByType::set_socket(AbstractSocket* socket) {
        if (m_error != Format::HCI::AttributeErrorCode::None) {
          return;
        }

        auto hci_socket = dynamic_cast<HCISocket*>(socket);
        if (hci_socket == nullptr) {
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't downcast socket to HCISocket packet");
        }

        uint8_t total_length = 1;  // 1 byte for opcode
        for (auto& service : hci_socket->get_services()) {
          if (service.handle >= m_starting_handle && service.handle <= m_ending_handle && service.uuid == m_value){
            total_length += 4;
            if (total_length > ATT_MTU) break;

            m_services.push_back(service);
          }
        }

        if (m_services.empty()) {
          m_error = Format::HCI::AttributeErrorCode::AttributeNotFound;
        }
      }

      const string FindByType::stringify() const {
        stringstream result;

        result << "<FindByType (Peripheral)> "
               << AbstractPacket::stringify() << ", "
               << "Starting handle: " << to_string(m_starting_handle) << ", "
               << "Ending handle: " << to_string(m_ending_handle) << ", "
               << "UUID: " << to_string(m_uuid_num) << ", "
               << "Value: " << Utils::format_uuid(m_value) << ", ";

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
