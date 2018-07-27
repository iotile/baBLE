#ifndef BABLE_PROBECHARACTERISTICS_HPP
#define BABLE_PROBECHARACTERISTICS_HPP

#include "Application/Packets/Base/HostOnlyPacket.hpp"
#include "Application/Packets/Commands/ReadByType/Central/ReadByTypeRequest.hpp"

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
      void _merge_characteristics();

      bool m_waiting_char_declaration;
      bool m_waiting_char_configuration;

      uint16_t m_start_handle;
      uint16_t m_end_handle;

      std::shared_ptr<Commands::Central::ReadByTypeRequest> m_read_by_type_request_packet;
      std::vector<Format::HCI::Characteristic> m_characteristics;
      std::vector<Format::HCI::Characteristic> m_characteristics_config;

    };

  }

}

#endif //BABLE_PROBECHARACTERISTICS_HPP
