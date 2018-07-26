#ifndef BABLE_PERIPHERAL_WRITEREQUEST_HPP
#define BABLE_PERIPHERAL_WRITEREQUEST_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class WriteRequest : public ControllerToHostPacket {

      public:
        static const Packet::Type initial_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return Format::HCI::AttributeCode::WriteRequest;
        };

        static const uint16_t final_packet_code() {
          return static_cast<uint16_t>(BaBLE::Payload::WritePeripheral);
        };

        explicit WriteRequest(uint16_t attribute_handle = 0, std::vector<uint8_t> value = {});

        void unserialize(HCIFormatExtractor& extractor) override;

        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;
        std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

        void prepare(const std::shared_ptr<PacketRouter>& router) override;

        void set_socket(AbstractSocket* socket) override;

        const std::string stringify() const override;

      private:
        uint16_t m_attribute_handle;
        std::vector<uint8_t> m_value;

        Format::HCI::AttributeErrorCode m_error;
      };

    }

  }

}

#endif //BABLE_PERIPHERAL_WRITEREQUEST_HPP
