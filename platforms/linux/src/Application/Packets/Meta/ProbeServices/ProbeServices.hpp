#ifndef BABLE_LINUX_PROBESERVICES_HPP
#define BABLE_LINUX_PROBESERVICES_HPP

#include "../../Commands/ReadByGroupType/ReadByGroupTypeRequest.hpp"

namespace Packet::Meta {

  class ProbeServices : public AbstractPacket {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'ProbeServices' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          throw std::invalid_argument("'ProbeServices' packet is not compatible with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::ProbeServices;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::ProbeServices);

        case Packet::Type::NONE:
          return 0;
      }
    };

    ProbeServices(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

    void before_sent(const std::shared_ptr<PacketRouter>& router) override;
    std::shared_ptr<AbstractPacket> on_read_by_group_type_response_received(const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<AbstractPacket>& packet);
    std::shared_ptr<AbstractPacket> on_error_response_received(const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<AbstractPacket>& packet);

  private:
    bool m_waiting_services;

    std::shared_ptr<Packet::Commands::ReadByGroupTypeRequest> m_read_by_type_group_request_packet;
    std::vector<Format::HCI::Service> m_services;

  };

}

#endif //BABLE_LINUX_PROBESERVICES_HPP
