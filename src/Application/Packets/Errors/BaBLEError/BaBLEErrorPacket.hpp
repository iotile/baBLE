#ifndef BABLE_LINUX_BABLEERRORPACKET_HPP
#define BABLE_LINUX_BABLEERRORPACKET_HPP

#include "../../../AbstractPacket.hpp"
#include "../../../../Exceptions/AbstractException.hpp"

namespace Packet::Errors {

  class BaBLEErrorPacket : public AbstractPacket {

  public:
    explicit BaBLEErrorPacket(Packet::Type output_type);

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    void from_exception(const Exceptions::AbstractException& exception);

  private:
    Exceptions::Type m_type;
    std::string m_message;
    std::string m_name;

  };

}

#endif //BABLE_LINUX_BABLEERRORPACKET_HPP
