#ifndef BABLE_PERIPHERAL_READREQUEST_HPP
#define BABLE_PERIPHERAL_READREQUEST_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class ReadRequest : public ControllerToHostPacket {

      public:
        static const Packet::Type initial_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return Format::HCI::AttributeCode::ReadRequest;
        };

        static const uint16_t final_packet_code() {
          return static_cast<uint16_t>(BaBLE::Payload::ReadPeripheral);
        };

        explicit ReadRequest(uint16_t attribute_handle = 0);

        void unserialize(HCIFormatExtractor& extractor) override;

        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;
        std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

        void prepare(const std::shared_ptr<PacketRouter>& router) override;

        const std::string stringify() const override;

      private:
        uint16_t m_starting_handle;
        uint16_t m_ending_handle;
        std::vector<uint8_t> m_uuid;

        bool m_read_attribute;
        uint16_t m_uuid_num;
        Format::HCI::AttributeErrorCode m_error;

        std::vector<Format::HCI::Characteristic> m_characteristics;
        uint8_t m_length_per_characteristic;
      };

    }

  }

}

#endif //BABLE_PERIPHERAL_READREQUEST_HPP
