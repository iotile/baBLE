#ifndef BABLE_EXIT_HPP
#define BABLE_EXIT_HPP

#include "Application/Packets/Base/HostOnlyPacket.hpp"

namespace Packet {

  namespace Control {

    class Exit : public HostOnlyPacket {

    public:
      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::Exit);
      };

      Exit();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      void set_socket(AbstractSocket* socket) override;

      const std::string stringify() const override;

    };

  }

}

#endif //BABLE_EXIT_HPP
