#ifndef BABLE_LINUX_GETCONTROLLERSLIST_HPP
#define BABLE_LINUX_GETCONTROLLERSLIST_HPP

#include "../CommandPacket.hpp"

namespace Packet::Commands {

  class GetControllersList : public CommandPacket<GetControllersList> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::GetControllersList;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::GetControllersList;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::GetControllersList);

        case Packet::Type::NONE:
          return 0;
      }
    };

    GetControllersList(Packet::Type initial_type, Packet::Type translated_type);

    void import(AsciiFormatExtractor& extractor) override;
    void import(FlatbuffersFormatExtractor& extractor) override;
    void import(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    std::vector<uint16_t> m_controllers;

  };

}

#endif //BABLE_LINUX_GETCONTROLLERSLIST_HPP
