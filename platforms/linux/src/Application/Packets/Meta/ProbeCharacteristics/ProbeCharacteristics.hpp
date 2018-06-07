#ifndef BABLE_LINUX_PROBECHARACTERISTICS_HPP
#define BABLE_LINUX_PROBECHARACTERISTICS_HPP

#include "../../Base/HostOnlyPacket.hpp"
#include "../../Commands/ReadByType/ReadByTypeRequest.hpp"

namespace Packet {

  namespace Meta {

    class ProbeCharacteristics : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::ProbeCharacteristics);
      };

      ProbeCharacteristics();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void prepare(const std::shared_ptr<PacketRouter>& router) override;
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
