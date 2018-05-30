#ifndef BABLE_LINUX_ERRORRESPONSE_HPP
#define BABLE_LINUX_ERRORRESPONSE_HPP

#include "../../../AbstractPacket.hpp"

namespace Packet::Errors {

  class ErrorResponse : public AbstractPacket {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'ErrorResponse' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::AttributeCode::ErrorResponse;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::ErrorResponse;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::ErrorResponse);

        case Packet::Type::NONE:
          return 0;
      }
    };

    ErrorResponse(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(HCIFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    inline uint8_t get_opcode() const {
      return m_opcode;
    };

    inline uint16_t get_handle() const {
      return m_handle;
    };

    inline Format::HCI::AttributeErrorCode get_error_code() const {
      return m_error_code;
    };

  private:
    uint8_t m_opcode;
    uint16_t m_handle;
    Format::HCI::AttributeErrorCode m_error_code;

  };

}

#endif //BABLE_LINUX_ERRORRESPONSE_HPP
