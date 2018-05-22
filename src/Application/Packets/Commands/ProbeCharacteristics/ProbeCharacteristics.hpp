#ifndef BABLE_LINUX_PROBECHARACTERISTICS_HPP
#define BABLE_LINUX_PROBECHARACTERISTICS_HPP

#include "../CommandPacket.hpp"

namespace Packet::Commands {

  class ProbeCharacteristics : public CommandPacket<ProbeCharacteristics> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          throw std::invalid_argument("'ProbeCharacteristics' packet is not compatible with MGMT protocol.");

        case Packet::Type::HCI:
          return Format::HCI::AttributeCode::ReadByTypeRequest;

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::ProbeCharacteristics;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::ProbeCharacteristics);

        case Packet::Type::NONE:
          return 0;
      }
    };

    ProbeCharacteristics(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;
    void unserialize(HCIFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

    void translate() override;
    std::vector<ResponseId> expected_response_ids() override;
    bool on_response_received(Packet::Type packet_type, const std::shared_ptr<AbstractExtractor>& extractor) override;

  private:
    uint16_t m_connection_handle;
    uint16_t m_starting_handle;
    uint16_t m_ending_handle;

    bool m_waiting_characteristics;
    std::vector<Format::HCI::Characteristic> m_characteristics;

  };

}

#endif //BABLE_LINUX_PROBECHARACTERISTICS_HPP
