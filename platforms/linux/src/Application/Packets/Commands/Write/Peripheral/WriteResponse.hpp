#ifndef BABLE_PERIPHERAL_WRITERESPONSE_HPP
#define BABLE_PERIPHERAL_WRITERESPONSE_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class WriteResponse : public HostToControllerPacket {

      public:
        static const Packet::Type final_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return static_cast<uint16_t>(BaBLE::Payload::WritePeripheral);
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::WriteResponse;
        };

        WriteResponse();

        void unserialize(FlatbuffersFormatExtractor& extractor) override;
        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        const std::string stringify() const override;

      private:
        uint16_t m_attribute_handle;

      };

    }

  }

}

#endif //BABLE_PERIPHERAL_WRITERESPONSE_HPP
