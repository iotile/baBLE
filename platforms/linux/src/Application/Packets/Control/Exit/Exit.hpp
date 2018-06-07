#ifndef BABLE_LINUX_EXIT_HPP
#define BABLE_LINUX_EXIT_HPP

#include "../../Base/HostOnlyPacket.hpp"

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

      const std::string stringify() const override;

    };

  }

}

#endif //BABLE_LINUX_EXIT_HPP
