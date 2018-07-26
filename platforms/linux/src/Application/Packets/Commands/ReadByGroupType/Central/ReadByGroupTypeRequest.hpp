#ifndef BABLE_CENTRAL_READBYGROUPTYPEREQUEST_HPP
#define BABLE_CENTRAL_READBYGROUPTYPEREQUEST_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Central {

      class ReadByGroupTypeRequest : public HostToControllerPacket {

      public:
        static const Packet::Type final_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          throw std::runtime_error("'Central::ReadByGroupTypeRequest' has no initial packet code (can't be registered).");
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::ReadByGroupTypeRequest;
        };

        explicit ReadByGroupTypeRequest(uint16_t starting_handle = 0x0001,
                                        uint16_t ending_handle = 0xFFFF,
                                        uint16_t uuid = Format::HCI::GattUUID::PrimaryServiceDeclaration);

        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        const std::string stringify() const override;

        void prepare(const std::shared_ptr<PacketRouter>& router) override;
        std::shared_ptr<AbstractPacket> on_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                     const std::shared_ptr<AbstractPacket>& packet) override;
        std::shared_ptr<AbstractPacket> on_error_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                           const std::shared_ptr<AbstractPacket>& packet);

        void set_handles(uint16_t starting_handle, uint16_t ending_handle);

      private:
        uint16_t m_starting_handle;
        uint16_t m_ending_handle;
        uint16_t m_uuid;
      };

    }

  }

}


#endif //BABLE_CENTRAL_READBYGROUPTYPEREQUEST_HPP
