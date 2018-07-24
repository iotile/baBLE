#ifndef BABLE_FINDINFORMATION_HPP
#define BABLE_FINDINFORMATION_HPP

#include <map>
#include "Application/Packets/Base/ControllerOnlyPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Peripheral {

      class FindInformation : public ControllerOnlyPacket {

      public:
        static const Packet::Type initial_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return Format::HCI::AttributeCode::FindInformationRequest;
        };

        static const uint16_t final_packet_code() {
          return Format::HCI::AttributeCode::FindInformationResponse;
        };

        explicit FindInformation(uint16_t starting_handle = 0x0001, uint16_t ending_handle = 0xFFFF);

        void unserialize(HCIFormatExtractor& extractor) override;
        std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

        void set_socket(AbstractSocket* socket) override;

        const std::string stringify() const override;

      private:
        uint16_t m_starting_handle;
        uint16_t m_ending_handle;
        uint8_t m_format;

        Format::HCI::AttributeErrorCode m_error;

        std::map<uint16_t, std::vector<uint8_t>> m_info;
      };

    }

  }

}

#endif //BABLE_FINDINFORMATION_HPP
