#ifndef BABLE_SETDISCOVERABLERESPONSE_HPP
#define BABLE_SETDISCOVERABLERESPONSE_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetDiscoverableResponse : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return Format::MGMT::CommandCode::SetDiscoverable;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::SetDiscoverable);
      };

      SetDiscoverableResponse();

      void unserialize(MGMTFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      bool m_state;

    };

  }

}

#endif //BABLE_SETDISCOVERABLERESPONSE_HPP
