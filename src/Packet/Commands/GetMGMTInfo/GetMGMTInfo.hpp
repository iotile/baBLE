#ifndef BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP
#define BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP

#include "../CommandPacket.hpp"

namespace Packet::Commands {

  class GetMGMTInfo : public CommandPacket<GetMGMTInfo> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::GetMGMTInfo;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::GetMGMTInfo;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::GetMGMTInfo);

        case Packet::Type::NONE:
          return 0;
      }
    };

    GetMGMTInfo(Packet::Type initial_type, Packet::Type translated_type);

    void import(AsciiFormatExtractor& extractor) override;
    void import(FlatbuffersFormatExtractor& extractor) override;
    void import(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    uint8_t m_version;
    uint16_t m_revision;

  };

}

#endif //BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP
