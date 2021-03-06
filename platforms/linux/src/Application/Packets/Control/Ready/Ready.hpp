#ifndef BABLE_READY_HPP
#define BABLE_READY_HPP

#include "Application/Packets/Base/HostOnlyPacket.hpp"

namespace Packet {

  namespace Control {

    class Ready : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::Ready);
      };

      Ready();

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    };

  }

}

#endif //BABLE_READY_HPP
