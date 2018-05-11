#ifndef BABLE_LINUX_SETCONNECTABLE_HPP
#define BABLE_LINUX_SETCONNECTABLE_HPP

#include "../CommandPacket.hpp"
#include "../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

namespace Packet::Commands {

  class SetConnectable : public CommandPacket<SetConnectable> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::SetConnectable;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::SetConnectable;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::SetConnectable);

        case Packet::Type::NONE:
          return 0;
      }
    };

    SetConnectable(Packet::Type initial_type, Packet::Type translated_type);

    void import(AsciiFormatExtractor& extractor) override;
    void import(FlatbuffersFormatExtractor& extractor) override;
    void import(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    bool m_state;

  };

}

#endif //BABLE_LINUX_SETCONNECTABLE_HPP
