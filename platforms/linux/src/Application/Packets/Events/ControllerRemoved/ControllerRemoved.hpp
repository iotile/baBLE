#ifndef BABLE_LINUX_CONTROLLERREMOVED_HPP
#define BABLE_LINUX_CONTROLLERREMOVED_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class ControllerRemoved : public EventPacket<ControllerRemoved> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::EventCode::IndexRemoved;

          case Packet::Type::HCI:
            throw std::invalid_argument("'ControllerRemoved' packet is not compatible with HCI protocol.");

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::ControllerRemoved);

          case Packet::Type::NONE:
            return 0;
        }
      };

      ControllerRemoved(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    };

  }

}

#endif //BABLE_LINUX_CONTROLLERREMOVED_HPP
