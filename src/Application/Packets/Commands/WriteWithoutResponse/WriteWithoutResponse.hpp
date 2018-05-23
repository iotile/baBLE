#ifndef BABLE_LINUX_WRITEWITHOUTRESPONSE_HPP
#define BABLE_LINUX_WRITEWITHOUTRESPONSE_HPP

#include "../CommandPacket.hpp"

namespace Packet::Commands {

  class WriteWithoutResponse : public CommandPacket<WriteWithoutResponse> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'WriteWithoutResponse' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::AttributeCode::WriteCommand;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::WriteWithoutResponse;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::WriteWithoutResponse);

        case Packet::Type::NONE:
          return 0;
      }
    };

    WriteWithoutResponse(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

    std::vector<ResponseId> expected_response_ids() override {
      // WriteWithoutResponse doesn't wait for any acknowledgement
      return {};
    };

  private:
    uint16_t m_connection_handle;
    uint16_t m_attribute_handle;
    std::vector<uint8_t> m_data_to_write;

  };

}

#endif //BABLE_LINUX_WRITEWITHOUTRESPONSE_HPP
