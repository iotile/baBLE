#ifndef BABLE_PERIPHERAL_READBYTYPEREQUEST_HPP
#define BABLE_PERIPHERAL_READBYTYPEREQUEST_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"
#include "Application/Packets/Commands/Read/Peripheral/ReadRequest.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class ReadByTypeRequest : public ControllerToHostPacket {

      public:
        static const Packet::Type initial_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return Format::HCI::AttributeCode::ReadByTypeRequest;
        };

        static const uint16_t final_packet_code() {
          return static_cast<uint16_t>(BaBLE::Payload::ReadPeripheral);
        };

        explicit ReadByTypeRequest(uint16_t starting_handle = 0x0001, uint16_t ending_handle = 0xFFFF);

        void unserialize(HCIFormatExtractor& extractor) override;

        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;
        std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

        void prepare(const std::shared_ptr<PacketRouter>& router) override;
        std::shared_ptr<AbstractPacket> on_read_response_received(const std::shared_ptr<PacketRouter>& router,
                                                                  const std::shared_ptr<AbstractPacket>& packet);

        void set_socket(AbstractSocket* socket) override;

        const std::string stringify() const override;

      private:
        uint16_t m_starting_handle;
        uint16_t m_ending_handle;
        std::vector<uint8_t> m_uuid;

        uint16_t m_uuid_num;
        Format::HCI::AttributeErrorCode m_error;

        std::vector<Format::HCI::Characteristic> m_characteristics;
        uint8_t m_length_per_characteristic;

        bool m_read_attribute;
        std::unique_ptr<Commands::Peripheral::ReadRequest> m_read_packet_request;
        uint16_t m_attribute_handle;
        std::vector<uint8_t> m_value_read;
      };

    }

  }

}

#endif //BABLE_PERIPHERAL_READBYTYPEREQUEST_HPP
