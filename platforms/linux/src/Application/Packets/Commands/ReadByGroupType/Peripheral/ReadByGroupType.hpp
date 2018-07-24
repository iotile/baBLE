#ifndef BABLE_LINUX_PERIPHERAL_READBYGROUPTYPE_HPP
#define BABLE_LINUX_PERIPHERAL_READBYGROUPTYPE_HPP

#include "Application/Packets/Base/ControllerOnlyPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class ReadByGroupType : public ControllerOnlyPacket {

      public:
        static const Packet::Type initial_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return Format::HCI::AttributeCode::ReadByGroupTypeRequest;
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::ReadByGroupTypeResponse;
        };

        explicit ReadByGroupType(uint16_t starting_handle = 0x0001, uint16_t ending_handle = 0xFFFF);

        void unserialize(HCIFormatExtractor& extractor) override;
        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        void set_socket(AbstractSocket* socket) override;

        const std::string stringify() const override;

      private:
        uint16_t m_starting_handle;
        uint16_t m_ending_handle;
        std::vector<uint8_t> m_uuid;

        Format::HCI::AttributeErrorCode m_error;

        std::vector<Format::HCI::Service> m_services;
        uint8_t m_length_per_service;
      };

    }

  }

}


#endif //BABLE_LINUX_PERIPHERAL_READBYGROUPTYPE_HPP
