#ifndef BABLE_FINDBYTYPE_HPP
#define BABLE_FINDBYTYPE_HPP

#include "Application/Packets/Base/ControllerOnlyPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class FindByType : public ControllerOnlyPacket {

      public:
        static const Packet::Type initial_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return Format::HCI::AttributeCode::FindByTypeRequest;
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::FindByTypeResponse;
        };

        explicit FindByType(uint16_t starting_handle = 0x0001, uint16_t ending_handle = 0xFFFF);

        void unserialize(HCIFormatExtractor& extractor) override;
        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        const std::string stringify() const override;

        void set_services(const std::vector<Format::HCI::Service>& services);

      private:
        uint16_t m_starting_handle;
        uint16_t m_ending_handle;
        uint16_t m_uuid_num;
        std::vector<uint8_t> m_value;

        Format::HCI::AttributeErrorCode m_error;

        std::vector<Format::HCI::Service> m_services;
      };

    }

  }

}

#endif //BABLE_FINDBYTYPE_HPP
