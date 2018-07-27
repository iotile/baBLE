#ifndef BABLE_CONTROLLERADDED_HPP
#define BABLE_CONTROLLERADDED_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Events {

    class ControllerAdded : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return Format::MGMT::EventCode::IndexAdded;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::ControllerAdded);
      };

      ControllerAdded();

      void unserialize(MGMTFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    };

  }

}

#endif //BABLE_CONTROLLERADDED_HPP
