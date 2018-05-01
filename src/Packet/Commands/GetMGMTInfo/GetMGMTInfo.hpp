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

        default:
          throw std::runtime_error("Current type has no known id (GetMGMTInfo).");
      }
    };

    GetMGMTInfo(Packet::Type initial_type, Packet::Type translated_type)
        : CommandPacket(initial_type, translated_type) {
      m_version = 0;
      m_revision = 0;
    };

    void import(AsciiFormatExtractor& extractor) override {
      CommandPacket::import(extractor);
    };

    void import(FlatbuffersFormatExtractor& extractor) override {
      CommandPacket::import(extractor);
    };

    void import(MGMTFormatExtractor& extractor) override {
      CommandPacket::import(extractor);
      m_version = extractor.get_value<uint8_t>();
      m_revision = extractor.get_value<uint16_t>();
    };

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override {
      CommandPacket::serialize(builder);
      builder
          .set_name("GetMGMTInfo")
          .add("Version", m_version)
          .add("Revision", m_revision);

      return builder.build();
    };

    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override {
      CommandPacket::serialize(builder);
      auto payload = Schemas::CreateGetMGMTInfo(builder, m_version, m_revision);

      return builder.build(payload, Schemas::Payload::GetMGMTInfo);
    }

    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override {
      CommandPacket::serialize(builder);
      return builder.build();
    };

  private:
    uint8_t m_version;
    uint16_t m_revision;

  };

}

#endif //BABLE_LINUX_COMMANDS_GETMGMTINFO_HPP
