#ifndef BABLE_LINUX_COMMANDPACKET_HPP
#define BABLE_LINUX_COMMANDPACKET_HPP

#include "../../AbstractPacket.hpp"

namespace Packet::Commands {

  template<class T>
  class CommandPacket : public AbstractPacket {

  protected:
    CommandPacket(Packet::Type initial_type, Packet::Type translated_type)
        : AbstractPacket(initial_type, translated_type) {
      m_command_code = T::packet_code(m_current_type);
      m_response_received = false;
    };

    void after_translate() override {
      m_command_code = T::packet_code(m_current_type);
    };

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {
      builder.set_code(m_command_code);

      return {};
    };

    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override {
      builder.set_opcode(static_cast<uint8_t>(m_command_code));

      return {};
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      builder
          .add("Type", "Command")
          .add("Command code", m_command_code);

      return {};
    };

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      return {};
    };

    void unserialize(MGMTFormatExtractor& extractor) override {
      m_command_code = extractor.get_packet_code();
      set_status(extractor.get_value<uint8_t>());
    };

    void unserialize(HCIFormatExtractor& extractor) override {};

    void unserialize(AsciiFormatExtractor& extractor) override {
      m_command_code = extractor.get_packet_code();
    };

    void unserialize(FlatbuffersFormatExtractor& extractor) override {};

    std::vector<ResponseId> expected_response_ids() override {
      if (!m_response_received) {
        return {
          ResponseId{m_current_type, m_controller_id, m_command_code}
        };

      } else {
        return {};
      }
    };

    bool on_response_received(Packet::Type packet_type, const std::shared_ptr<AbstractExtractor>& extractor) override {
      if (packet_type != m_translated_type) {
        return false;
      }

      if (packet_type == Packet::Type::HCI){
        uint16_t type_code = extractor->get_type_code();
        uint16_t packet_code = extractor->get_packet_code();

        if (type_code == Format::HCI::AsyncData && packet_code == Format::HCI::ErrorResponse) {
          return on_data_error_received(packet_type, extractor);
        }
      }

      LOG.debug("Response received", "CommandPacket");
      from_bytes(extractor);
      m_response_received = true;
      return true;
    };

    bool on_data_error_received(Packet::Type packet_type, const std::shared_ptr<AbstractExtractor>& extractor) {
      auto opcode = extractor->get_value<uint16_t>();

      if (opcode != m_command_code)  {
        return false;
      }

      LOG.debug("Data error received", "CommandPacket");

      auto handle_in_error = extractor->get_value<uint16_t>();
      auto error_code = extractor->get_value<uint8_t>();
      set_status(error_code, false);
      m_native_class = "HCI-Attribute";

      switch (error_code) {
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

      m_response_received = true;
      return true;
    };

    uint16_t m_command_code;
    bool m_response_received;

  };

}

#endif //BABLE_LINUX_COMMANDPACKET_HPP