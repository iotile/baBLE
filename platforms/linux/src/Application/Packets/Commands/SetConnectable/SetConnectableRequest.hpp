#ifndef BABLE_LINUX_SETCONNECTABLEREQUEST_HPP
#define BABLE_LINUX_SETCONNECTABLEREQUEST_HPP

#include "../RequestPacket.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

namespace Packet::Commands {

  class SetConnectableRequest : public RequestPacket<SetConnectableRequest> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::SetConnectable;

        case Packet::Type::HCI:
          throw std::invalid_argument("'SetConnectable' packet is not compatible with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::SetConnectable;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::SetConnectable);

        case Packet::Type::NONE:
          return 0;
      }
    };

    SetConnectableRequest(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    bool m_state;

  };

}

#endif //BABLE_LINUX_SETCONNECTABLEREQUEST_HPP
