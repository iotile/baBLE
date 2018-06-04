#ifndef BABLE_LINUX_READY_HPP
#define BABLE_LINUX_READY_HPP

#include "../../../AbstractPacket.hpp"

namespace Packet {

  namespace Control {

    class Ready : public AbstractPacket {

    public:
      explicit Ready(Packet::Type output_type);

      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    };

  }

}

#endif //BABLE_LINUX_READY_HPP
