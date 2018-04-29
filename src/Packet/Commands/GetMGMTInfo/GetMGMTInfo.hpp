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
      m_version = 0;
      m_revision = 0;
    };

    void from_ascii(const std::vector<std::string>& params) override {};

    void from_flatbuffers(Deserializer& deser) override {};

    void from_mgmt(Deserializer& deser) override {
      CommandPacket::from_mgmt(deser);
      deser >> m_version >> m_revision;
    };

    std::string to_ascii() const override {
      std::string header = CommandPacket::to_ascii();
      std::stringstream payload;

      payload << "Version: " << std::to_string(m_version) << ", "
              << "Revision: " << std::to_string(m_revision);

      return header + ", " + payload.str();
    };

    Serializer to_flatbuffers() const override {
      flatbuffers::FlatBufferBuilder builder(0);
      auto payload = Schemas::CreateGetMGMTInfo(builder, m_version, m_revision);

      Serializer ser = build_flatbuffers_packet<Schemas::GetMGMTInfo>(builder, payload, Schemas::Payload::GetMGMTInfo);

      Serializer result;
      result << static_cast<uint8_t>(0xCA) << static_cast<uint8_t>(0xFE) << static_cast<uint16_t>(ser.size()) << ser;

      return result;
    }

    Serializer to_mgmt() const override {
      return CommandPacket::to_mgmt();
    };

  private:
    uint8_t m_version;
    uint16_t m_revision;

  };

}

#endif //BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP
