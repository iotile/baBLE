#ifndef BABLE_LINUX_GETCONTROLLERINFO_HPP
#define BABLE_LINUX_GETCONTROLLERINFO_HPP

#include "../CommandPacket.hpp"
#include "../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

namespace Packet::Commands {

  class GetControllerInfo : public CommandPacket<GetControllerInfo> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::GetControllerInfo;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::GetControllerInfo;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::GetControllerInfo);

        case Packet::Type::NONE:
          return 0;
      }
    };

    GetControllerInfo(Packet::Type initial_type, Packet::Type translated_type);

    void import(AsciiFormatExtractor& extractor) override;
    void import(FlatbuffersFormatExtractor& extractor) override;
    void import(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    std::array<uint8_t, 6> m_address{};
    uint8_t m_bluetooth_version;
    uint16_t m_manufacturer;
    uint32_t m_supported_settings;
    uint32_t m_current_settings;
    std::array<uint8_t, 3> m_class_of_device{};
    std::string m_name;
    std::string m_short_name;

  };

}


#endif //BABLE_LINUX_GETCONTROLLERINFO_HPP
