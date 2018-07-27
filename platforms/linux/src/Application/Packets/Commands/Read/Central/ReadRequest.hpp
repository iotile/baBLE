#ifndef BABLE_CENTRAL_READREQUEST_HPP
#define BABLE_CENTRAL_READREQUEST_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Central {

      class ReadRequest : public HostToControllerPacket {

      public:
        static const Packet::Type final_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return static_cast<uint16_t>(BaBLE::Payload::ReadCentral);
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::ReadRequest;
        };

        explicit ReadRequest(uint16_t attribute_handle = 0);

        void unserialize(FlatbuffersFormatExtractor& extractor) override;
        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        const std::string stringify() const override;

        void prepare(const std::shared_ptr<PacketRouter>& router) override;
        std::shared_ptr<AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                             const std::shared_ptr<AbstractPacket>& packet) override;
        std::shared_ptr<AbstractPacket> on_error_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                   const std::shared_ptr<AbstractPacket>& packet);

      private:
        uint16_t m_attribute_handle;
      };

    }

  }

}

#endif //BABLE_CENTRAL_READREQUEST_HPP
