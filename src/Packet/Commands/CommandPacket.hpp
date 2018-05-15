#ifndef BABLE_LINUX_COMMANDPACKET_HPP
#define BABLE_LINUX_COMMANDPACKET_HPP

#include "../AbstractPacket.hpp"

namespace Packet::Commands {

  template<class T>
  class CommandPacket : public AbstractPacket {

  public:
    std::tuple<Schemas::StatusCode, uint8_t, std::string> get_full_status() {
      return std::make_tuple(m_status, m_native_status, m_native_class);
    }

  protected:
    CommandPacket(Packet::Type initial_type, Packet::Type translated_type)
        : AbstractPacket(initial_type, translated_type) {
      m_command_code = T::command_code(current_type());
      m_controller_id = Format::MGMT::non_controller_id;
      m_status = Schemas::StatusCode::Success;
      m_native_status = 0;
    }

    void after_translate() override {
      m_command_code = T::command_code(current_type());
    }

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {
      builder.set_code(m_command_code);

      return {};
    }

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      std::string status_name = Schemas::EnumNameStatusCode(m_status);

      builder
          .add("Type", "Command")
          .add("Command code", m_command_code)
          .add("Status", status_name)
          .add("Native status", m_native_status);

      return {};
    }

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      return {};
    }

    void unserialize(MGMTFormatExtractor& extractor) override {
      m_command_code = extractor.get_value<uint16_t>();
      m_native_status = extractor.get_value<uint8_t>();
      m_native_class = "MGMT";

      switch (m_native_status) {
        case Format::MGMT::Success:
          m_status = Schemas::StatusCode::Success;
          break;

        case Format::MGMT::Busy:
        case Format::MGMT::Rejected:
        case Format::MGMT::NotSupported:
        case Format::MGMT::AlreadyPaired:
          m_status = Schemas::StatusCode::Rejected;
          break;

        case Format::MGMT::PermissionDenied:
          m_status = Schemas::StatusCode::Denied;
          break;

        case Format::MGMT::Cancelled:
          m_status = Schemas::StatusCode::Cancelled;
          break;

        case Format::MGMT::NotPowered:
          m_status = Schemas::StatusCode::NotPowered;
          break;

        case Format::MGMT::Failed:
        case Format::MGMT::ConnectFailed:
        case Format::MGMT::AuthenticationFailed:
        case Format::MGMT::RFKilled:
          m_status = Schemas::StatusCode::Failed;
          break;

        case Format::MGMT::UnknownCommand:
        case Format::MGMT::InvalidParameters:
        case Format::MGMT::InvalidIndex:
          m_status = Schemas::StatusCode::InvalidCommand;
          break;

        case Format::MGMT::NotConnected:
          m_status = Schemas::StatusCode::NotConnected;
          break;

        default:
          m_status = Schemas::StatusCode::Unknown;
          break;
      }
    }

    void unserialize(AsciiFormatExtractor& extractor) override {
      m_command_code = extractor.get_command_code();
    }

    void unserialize(FlatbuffersFormatExtractor& extractor) override {}

    uint16_t m_command_code;
    uint8_t m_native_status;
    std::string m_native_class;
    Schemas::StatusCode m_status;

  };

}

#endif //BABLE_LINUX_COMMANDPACKET_HPP