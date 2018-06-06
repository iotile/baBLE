#ifndef BABLE_LINUX_CANCELCONNECTIONRESPONSE_HPP
#define BABLE_LINUX_CANCELCONNECTIONRESPONSE_HPP

#include "../../Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    class CancelConnectionResponse : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::CommandCode::LECreateConnectionCancel;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::CancelConnection);
      };

      CancelConnectionResponse();

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    };

  }

}

#endif //BABLE_LINUX_CANCELCONNECTIONRESPONSE_HPP
