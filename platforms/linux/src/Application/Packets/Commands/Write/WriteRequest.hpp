#ifndef BABLE_LINUX_WRITEREQUEST_HPP
#define BABLE_LINUX_WRITEREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet::Commands {

  class WriteRequest: public RequestPacket<WriteRequest> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'Write' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::AttributeCode::WriteRequest;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::Write;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::Write);

        case Packet::Type::NONE:
          return 0;
      }
    };

    WriteRequest(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;
    void unserialize(HCIFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

    void before_sent(const std::shared_ptr<PacketRouter>& router) override;
    std::shared_ptr<Packet::AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<AbstractPacket>& packet) override;
    std::shared_ptr<Packet::AbstractPacket> on_error_response_received(const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<AbstractPacket>& packet);

  private:
    uint16_t m_attribute_handle;
    std::vector<uint8_t> m_data_to_write;

  };

}

#endif //BABLE_LINUX_WRITEREQUEST_HPP
