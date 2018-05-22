#ifndef BABLE_LINUX_PROBESERVICES_HPP
#define BABLE_LINUX_PROBESERVICES_HPP

#include "../CommandPacket.hpp"

namespace Packet::Commands {

  class ProbeServices : public CommandPacket<ProbeServices> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'ProbeServices' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::AttributeCode::ReadByGroupTypeRequest;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::ProbeServices;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::ProbeServices);

        case Packet::Type::NONE:
          return 0;
      }
    };

    ProbeServices(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;
    void unserialize(HCIFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

    void translate() override;
    std::vector<uint64_t> expected_response_uuids() override;

  private:
    uint16_t m_connection_handle;
    uint16_t m_starting_handle;
    uint16_t m_ending_handle;

    bool m_waiting_services;
    std::vector<Format::HCI::Service> m_services;

  };

}

#endif //BABLE_LINUX_PROBESERVICES_HPP
