#ifndef BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP
#define BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP

#include <cstdint>
#include <flatbuffers/flatbuffers.h>
#include <Packet_generated.h>
#include "../CommandPacket.hpp"
#include "../../constants.hpp"

namespace Packet::Commands {

  class GetMGMTInfo : public CommandPacket<GetMGMTInfo> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Commands::MGMT::Code::GetMGMTInfo;

        case Packet::Type::ASCII:
          return Commands::Ascii::Code::GetMGMTInfo;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::GetMGMTInfo);

        default:
          throw std::runtime_error("Current type has no known id (GetMGMTInfo).");
      }
    };

    GetMGMTInfo(Packet::Type initial_type, Packet::Type translated_type): CommandPacket(initial_type, translated_type) {
      m_command_code = command_code(m_current_type);
    };

    void from_ascii(const std::vector<std::string>& params) override {};

    void from_flatbuffers(Deserializer& deser) override {};

    std::string to_ascii() const override {
      return CommandPacket::to_ascii();
    };

    Serializer to_mgmt() const override {
      return CommandPacket::to_mgmt();
    };

  };

}

#endif //BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP
