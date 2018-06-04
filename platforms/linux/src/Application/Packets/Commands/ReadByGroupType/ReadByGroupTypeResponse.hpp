#ifndef BABLE_LINUX_READBYGROUPTYPERESPONSE_HPP
#define BABLE_LINUX_READBYGROUPTYPERESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    class ReadByGroupTypeResponse : public ResponsePacket<ReadByGroupTypeResponse> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'ReadByGroupTypeResponse' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::AttributeCode::ReadByGroupTypeResponse;

          case Packet::Type::FLATBUFFERS:
            throw std::invalid_argument("'ReadByGroupTypeResponse' packet is not compatible with Flatbuffers protocol.");

          case Packet::Type::NONE:
            return 0;
        }
      };

      ReadByGroupTypeResponse(Packet::Type initial_type, Packet::Type final_type);

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

#endif //BABLE_LINUX_READBYGROUPTYPERESPONSE_HPP
