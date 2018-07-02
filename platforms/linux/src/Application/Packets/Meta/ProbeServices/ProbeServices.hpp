#ifndef BABLE_LINUX_PROBESERVICES_HPP
#define BABLE_LINUX_PROBESERVICES_HPP

#include "Application/Packets/Base/HostOnlyPacket.hpp"
#include "Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeRequest.hpp"

namespace Packet {

  namespace Meta {

    class ProbeServices : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::ProbeServices);
      };

      ProbeServices();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

      void prepare(const std::shared_ptr<PacketRouter>& router) override;
      std::shared_ptr<AbstractPacket> on_read_by_group_type_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                              const std::shared_ptr<AbstractPacket>& packet);
      std::shared_ptr<AbstractPacket> on_error_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                 const std::shared_ptr<AbstractPacket>& packet);

    private:
      bool m_waiting_services;

      std::shared_ptr<Packet::Commands::ReadByGroupTypeRequest> m_read_by_type_group_request_packet;
      std::vector<Format::HCI::Service> m_services;

    };

  }

}

#endif //BABLE_LINUX_PROBESERVICES_HPP
