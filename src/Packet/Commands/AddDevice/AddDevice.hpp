#ifndef BABLE_LINUX_CONNECT_HPP
#define BABLE_LINUX_CONNECT_HPP

#include "../CommandPacket.hpp"
#include "../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

namespace Packet::Commands {

  class AddDevice : public CommandPacket<AddDevice> {

  public:
    static const uint16_t command_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::AddDevice;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::AddDevice;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::AddDevice);
      }
    };

    AddDevice(Packet::Type initial_type, Packet::Type translated_type);

    void import(AsciiFormatExtractor& extractor) override;
    void import(FlatbuffersFormatExtractor& extractor) override;
    void import(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    uint8_t m_address_type;
    std::array<uint8_t, 6> m_address{};

  };

}

#endif //BABLE_LINUX_CONNECT_HPP
