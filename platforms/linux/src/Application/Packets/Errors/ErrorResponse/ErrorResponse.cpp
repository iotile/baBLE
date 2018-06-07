#include "ErrorResponse.hpp"

using namespace std;

namespace Packet {

  namespace Errors {

    ErrorResponse::ErrorResponse(Packet::Type initial_type, Packet::Type translated_type)
        : AbstractPacket(initial_type, translated_type) {
      m_id = Packet::Id::ErrorResponse;
      m_packet_code = packet_code(m_current_type);
      m_opcode = 0;
      m_handle = 0;
      m_error_code = Format::HCI::AttributeErrorCode::None;
    }

    void ErrorResponse::unserialize(HCIFormatExtractor& extractor) {
      m_opcode = extractor.get_value<uint8_t>();
      m_handle = extractor.get_value<uint16_t>();

      try {
        m_error_code = static_cast<Format::HCI::AttributeErrorCode>(extractor.get_value<uint8_t>());

      } catch (const std::bad_cast& err) {
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

    vector<uint8_t> ErrorResponse::serialize(AsciiFormatBuilder& builder) const {
      builder
          .set_name("ErrorResponse")
          .add("Request opcode in error", m_opcode)
          .add("Handle in error", m_handle)
          .add("Error code", m_error_code);

      return builder.build();
    }

    vector<uint8_t> ErrorResponse::serialize(FlatbuffersFormatBuilder& builder) const {
      auto payload = BaBLE::CreateErrorResponse(builder, m_opcode, m_handle);

      return builder
          .set_status(m_status)
          .build(payload, BaBLE::Payload::ErrorResponse);
    }

  }

}