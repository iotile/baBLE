#ifndef BABLE_CENTRAL_READBYGROUPTYPERESPONSE_HPP
#define BABLE_CENTRAL_READBYGROUPTYPERESPONSE_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    namespace Central {

      class ReadByGroupTypeResponse : public ControllerToHostPacket {

      public:
        static const Packet::Type initial_type() {
          return Packet::Type::HCI;
        };

        static const uint16_t initial_packet_code() {
          return Format::HCI::AttributeCode::ReadByGroupTypeResponse;
        };

        static const uint16_t final_packet_code() {
          return initial_packet_code();
        };

        ReadByGroupTypeResponse();

        void unserialize(HCIFormatExtractor& extractor) override;

        const std::string stringify() const override;

        inline std::vector<Format::HCI::Service> get_services() const {
          return m_services;
        };

        inline uint16_t get_last_group_end_handle() const {
          return m_last_group_end_handle;
        };

      private:
        std::vector<Format::HCI::Service> m_services;
        uint16_t m_last_group_end_handle;

      };

    }

  }

}

#endif //BABLE_CENTRAL_READBYGROUPTYPERESPONSE_HPP
