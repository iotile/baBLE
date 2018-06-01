#ifndef BABLE_LINUX_COMMANDCOMPLETE_HPP
#define BABLE_LINUX_COMMANDCOMPLETE_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class CommandComplete : public EventPacket<CommandComplete> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'CommandComplete' packet is not compatible with HCI protocol.");

          case Packet::Type::HCI:
            return Format::HCI::EventCode::CommandComplete;

          case Packet::Type::ASCII:
            return Format::Ascii::EventCode::CommandComplete;

          case Packet::Type::FLATBUFFERS:
            throw std::invalid_argument("'CommandComplete' event has no event code for Flatbuffers.");

          case Packet::Type::NONE:
            return 0;
        }
      };

      CommandComplete(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(HCIFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;

    };

  }

}

#endif //BABLE_LINUX_COMMANDCOMPLETE_HPP
