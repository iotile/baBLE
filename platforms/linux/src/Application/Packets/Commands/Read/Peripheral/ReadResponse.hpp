#ifndef BABLE_PERIPHERAL_READRESPONSE_HPP
#define BABLE_PERIPHERAL_READRESPONSE_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class ReadResponse : public HostToControllerPacket {

      public:
        static const Packet::Type final_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return static_cast<uint16_t>(BaBLE::Payload::ReadPeripheral);
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::ReadResponse;
        };

        ReadResponse();

        void unserialize(FlatbuffersFormatExtractor& extractor) override;
        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        inline uint16_t get_attribute_handle() const {
          return m_attribute_handle;
        };

        inline std::vector<uint8_t> get_value() const {
          return m_value;
        };

        const std::string stringify() const override;

      private:
        uint16_t m_attribute_handle;
        std::vector<uint8_t> m_value;

      };

    }

  }

}

#endif //BABLE_PERIPHERAL_READRESPONSE_HPP
