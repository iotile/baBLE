#ifndef BABLE_LINUX_READBYTYPERESPONSE_HPP
#define BABLE_LINUX_READBYTYPERESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    class ReadByTypeResponse : public ResponsePacket<ReadByTypeResponse> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'ReadByTypeResponse' packet is not compatible with MGMT protocol.");

          case Packet::Type::HCI:
            return Format::HCI::AttributeCode::ReadByTypeResponse;

          case Packet::Type::FLATBUFFERS:
            throw std::invalid_argument("'ReadByTypeResponse' packet is not compatible with Flatbuffers protocol.");

          case Packet::Type::NONE:
            return 0;
        }
      };

      ReadByTypeResponse(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(HCIFormatExtractor& extractor) override;

      const std::string stringify() const override;

      inline std::vector<Format::HCI::Characteristic> get_characteristics() const {
        return m_characteristics;
      };

      inline uint16_t get_last_ending_handle() const {
        return m_last_ending_handle;
      };

    private:
      std::vector<Format::HCI::Characteristic> m_characteristics;
      uint16_t m_last_ending_handle;

    };

  }

}

#endif //BABLE_LINUX_READBYTYPERESPONSE_HPP
