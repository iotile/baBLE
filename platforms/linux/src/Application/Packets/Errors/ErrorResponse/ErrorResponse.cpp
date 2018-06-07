#include <sstream>
#include "ErrorResponse.hpp"

using namespace std;

namespace Packet {

  namespace Errors {

    ErrorResponse::ErrorResponse()
        : ControllerToHostPacket(Packet::Id::ErrorResponse, initial_type(), initial_packet_code(), final_packet_code(), true) {
      m_opcode = 0;
      m_handle = 0;
      m_error_code = Format::HCI::AttributeErrorCode::None;
    }

    void ErrorResponse::unserialize(HCIFormatExtractor& extractor) {
      m_opcode = extractor.get_value<uint8_t>();
      m_handle = extractor.get_value<uint16_t>();

      try {
        m_error_code = static_cast<Format::HCI::AttributeErrorCode>(extractor.get_value<uint8_t>());

      } catch (const bad_cast& err) {
        throw Exceptions::WrongFormatException("Unknown error code received in ErrorResponse", m_uuid_request);
      }

      set_status(m_error_code, false);

      switch (m_error_code) {
        case Format::HCI::AttributeErrorCode::ReadNotPermitted:
        case Format::HCI::AttributeErrorCode::WriteNotPermitted:
        case Format::HCI::AttributeErrorCode::InsufficientAuthentication:
        case Format::HCI::AttributeErrorCode::InsufficientAuthorization:
        case Format::HCI::AttributeErrorCode::InsufficientEncryption:
          m_status = BaBLE::StatusCode::Denied;
          break;

        case Format::HCI::AttributeErrorCode::InvalidHandle:
        case Format::HCI::AttributeErrorCode::InvalidPDU:
        case Format::HCI::AttributeErrorCode::RequestNotSupported:
        case Format::HCI::AttributeErrorCode::InvalidOffset:
        case Format::HCI::AttributeErrorCode::InvalidAttributeValueLength:
        case Format::HCI::AttributeErrorCode::AttributeNotFound:
        case Format::HCI::AttributeErrorCode::AttributeNotLong:
          m_status = BaBLE::StatusCode::InvalidCommand;
          break;

        case Format::HCI::AttributeErrorCode::PrepareQueueFull:
        case Format::HCI::AttributeErrorCode::UnlikelyError:
        case Format::HCI::AttributeErrorCode::UnsupportedGroupType:
        case Format::HCI::AttributeErrorCode::InsufficientResources:
          m_status = BaBLE::StatusCode::Failed;
          break;

        default:
          m_status = BaBLE::StatusCode::Unknown;
      }
    }

    const std::string ErrorResponse::stringify() const {
      stringstream result;

      result << "<ErrorResponse> "
             << AbstractPacket::stringify() << ", "
             << "Request opcode in error: " << to_string(m_opcode) << ", "
             << "Handle in error: " << to_string(m_handle) << ", "
             << "Error code: " << to_string(m_error_code);

      return result.str();
    }

    const PacketUuid ErrorResponse::get_uuid() const {
      return PacketUuid{
          m_current_type,
          m_controller_id,
          m_connection_id,
          m_packet_code,
          get_opcode()
      };
    }

  }

}