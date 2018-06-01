#ifndef BABLE_LINUX_GETCONNECTEDDEVICESREQUEST_HPP
#define BABLE_LINUX_GETCONNECTEDDEVICESREQUEST_HPP

#include "../RequestPacket.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

namespace Packet::Commands {

  class GetConnectedDevicesRequest : public RequestPacket<GetConnectedDevicesRequest> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::GetConnections;

        case Packet::Type::HCI:
          throw std::invalid_argument("'GetConnectedDevices' packet is not compatible with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::GetConnectedDevices;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::GetConnectedDevices);

        case Packet::Type::NONE:
          return 0;
      }
    };

    GetConnectedDevicesRequest(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  };

}

#endif //BABLE_LINUX_GETCONNECTEDDEVICESREQUEST_HPP