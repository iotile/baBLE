#ifndef BABLE_PERIPHERAL_READBYTYPERESPONSE_HPP
#define BABLE_PERIPHERAL_READBYTYPERESPONSE_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class ReadByTypeResponse : public HostToControllerPacket {

      public:
        static const Packet::Type final_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return static_cast<uint16_t>(BaBLE::Payload::ReadPeripheral);
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::ReadByTypeResponse;
        };

        ReadByTypeResponse();

        void unserialize(FlatbuffersFormatExtractor& extractor) override;
        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        const std::string stringify() const override;

      private:
        uint16_t m_value_handle;
        std::vector<uint8_t> m_value;

      };

    }

  }

}

#endif //BABLE_PERIPHERAL_READBYTYPERESPONSE_HPP
