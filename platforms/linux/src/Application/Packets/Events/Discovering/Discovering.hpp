#ifndef BABLE_LINUX_DISCOVERING_HPP
#define BABLE_LINUX_DISCOVERING_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class Discovering : public EventPacket<Discovering> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::EventCode::Discovering;

          case Packet::Type::HCI:
            throw std::invalid_argument("'Discovering' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::EventCode::Discovering;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::Discovering);

          case Packet::Type::NONE:
            return 0;
        }
      };

      Discovering(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    private:
      uint8_t m_address_type;
      uint8_t m_discovering;
    };

  }

}

#endif //BABLE_LINUX_DISCOVERING_HPP
