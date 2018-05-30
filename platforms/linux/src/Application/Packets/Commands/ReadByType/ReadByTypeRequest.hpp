#ifndef BABLE_LINUX_READBYTYPEREQUEST_HPP
#define BABLE_LINUX_READBYTYPEREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet::Commands {

  class ReadByTypeRequest : public RequestPacket<ReadByTypeRequest> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'ReadByTypeRequest' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::AttributeCode::ReadByTypeRequest;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::ReadByTypeRequest;

        case Packet::Type::FLATBUFFERS:
          throw std::invalid_argument("'ReadByTypeRequest' packet is not compatible with Flatbuffers protocol.");

        case Packet::Type::NONE:
          return 0;
      }
    };

    ReadByTypeRequest(Packet::Type initial_type, Packet::Type translated_type);
    ReadByTypeRequest& set_handles(uint16_t starting_handle, uint16_t ending_handle);

    void unserialize(AsciiFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;

  private:
    uint16_t m_starting_handle;
    uint16_t m_ending_handle;
    uint16_t m_uuid;
  };

}

#endif //BABLE_LINUX_READBYTYPEREQUEST_HPP
