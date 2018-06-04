#ifndef BABLE_LINUX_PROBECHARACTERISTICS_HPP
#define BABLE_LINUX_PROBECHARACTERISTICS_HPP

#include "../../Commands/ReadByType/ReadByTypeRequest.hpp"

namespace Packet {

  namespace Meta {

    class ProbeCharacteristics : public AbstractPacket {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'ProbeCharacteristics' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            throw std::invalid_argument("'ProbeCharacteristics' packet is not compatible with HCI protocol.");

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::ProbeCharacteristics);

          case Packet::Type::NONE:
            return 0;
        }
      };

      ProbeCharacteristics(Packet::Type initial_type, Packet::Type final_type);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void before_sent(const std::shared_ptr<PacketRouter>& router) override;
      std::shared_ptr<AbstractPacket> on_read_by_type_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                        const std::shared_ptr<AbstractPacket>& packet);
      std::shared_ptr<AbstractPacket> on_error_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                 const std::shared_ptr<AbstractPacket>& packet);

    private:
      bool m_waiting_characteristics;

      std::shared_ptr<Packet::Commands::ReadByTypeRequest> m_read_by_type_request_packet;
      std::vector<Format::HCI::Characteristic> m_characteristics;

    };

  }

}

#endif //BABLE_LINUX_PROBECHARACTERISTICS_HPP
