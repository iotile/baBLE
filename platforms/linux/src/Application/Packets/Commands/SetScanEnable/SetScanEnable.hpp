#ifndef BABLE_LINUX_SETSCANENABLE_HPP
#define BABLE_LINUX_SETSCANENABLE_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetScanEnable : public RequestPacket<SetScanEnable> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'SetScanEnable' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::CommandCode::SetScanEnable;

          case Packet::Type::ASCII:
            return Format::Ascii::CommandCode::SetScanEnable;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::StopScan);

          case Packet::Type::NONE:
            return 0;
        }
      };

      SetScanEnable(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(AsciiFormatExtractor& extractor) override;
      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      std::shared_ptr<Packet::AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                   const std::shared_ptr<AbstractPacket>& packet) override;

      void set_state(bool state);

    private:
      bool m_state;
      bool m_filter_duplicates;
    };

  }

}

#endif //BABLE_LINUX_SETSCANENABLE_HPP
