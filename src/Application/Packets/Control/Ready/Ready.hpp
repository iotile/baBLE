#ifndef BABLE_LINUX_READY_HPP
#define BABLE_LINUX_READY_HPP

#include "../../../AbstractPacket.hpp"

namespace Packet::Control {

  class Ready : public AbstractPacket {

  public:
    explicit Ready(Packet::Type output_type);

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

  };

}
#endif //BABLE_LINUX_READY_HPP
