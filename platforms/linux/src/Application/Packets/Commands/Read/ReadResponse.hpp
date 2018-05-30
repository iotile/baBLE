#ifndef BABLE_LINUX_READRESPONSE_HPP
#define BABLE_LINUX_READRESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet::Commands {

  class ReadResponse : public ResponsePacket<ReadResponse> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'Read' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::AttributeCode::ReadResponse;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::Read;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::Read);

        case Packet::Type::NONE:
          return 0;
      }
    };

    ReadResponse(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(HCIFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    inline void set_attribute_handle(uint16_t attribute_handle) {
      m_attribute_handle = attribute_handle;
    };

  private:
    uint16_t m_attribute_handle;
    std::vector<uint8_t> m_data_read;

  };

}

#endif //BABLE_LINUX_READRESPONSE_HPP
