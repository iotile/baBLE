#ifndef BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP
#define BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP

#include <cstdint>
#include "../CommandPacket.hpp"
#include "../../constants.hpp"

namespace Packet::Commands {

  class GetMGMTInfo : public CommandPacket {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Commands::MGMT::Code::GetMGMTInfo;

        case Packet::Type::ASCII:
          return Commands::Ascii::Code::GetMGMTInfo;

        default:
          throw std::runtime_error("Current type has no known id.");
      }
    };

    GetMGMTInfo(Packet::Type initial_type, Packet::Type translated_type): CommandPacket(initial_type, translated_type) {
      m_command_code = command_code(m_current_type);
    };

    void from_ascii() override {};

    Serializer to_mgmt() const override {
      Serializer ser;
      generate_header(ser);
      return ser;
    };

  };

}

#endif //BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP
