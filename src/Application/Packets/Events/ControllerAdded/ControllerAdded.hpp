#ifndef BABLE_LINUX_CONTROLLERADDED_HPP
#define BABLE_LINUX_CONTROLLERADDED_HPP

#include "../EventPacket.hpp"

namespace Packet::Events {

  class ControllerAdded : public EventPacket<ControllerAdded> {

  public:
    static const uint16_t event_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::EventCode::IndexAdded;

        case Packet::Type::ASCII:
          return Format::Ascii::EventCode::ControllerAdded;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(Schemas::Payload::ControllerAdded);

        case Packet::Type::NONE:
          return 0;
      }
    };

    ControllerAdded(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

  };

}

#endif //BABLE_LINUX_CONTROLLERADDED_HPP
