#ifndef BABLE_EMITNOTIFICATION_HPP
#define BABLE_EMITNOTIFICATION_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class EmitNotification : public HostToControllerPacket {

      public:
        static const Packet::Type final_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return static_cast<uint16_t>(BaBLE::Payload::EmitNotification);
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::HandleValueNotification;
        };

        explicit EmitNotification(uint16_t attribute_handle = 0, std::vector<uint8_t> value = {});

        void unserialize(FlatbuffersFormatExtractor& extractor) override;
        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        void set_socket(AbstractSocket* socket) override;

        const std::string stringify() const override;

      private:
        uint16_t m_attribute_handle;
        std::vector<uint8_t> m_value;

      };

    }

  }

}
#endif //BABLE_EMITNOTIFICATION_HPP
