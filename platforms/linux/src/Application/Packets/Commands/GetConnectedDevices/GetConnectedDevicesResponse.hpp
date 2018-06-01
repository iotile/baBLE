#ifndef BABLE_LINUX_GETCONNECTEDDEVICESRESPONSE_HPP
#define BABLE_LINUX_GETCONNECTEDDEVICESRESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet::Commands {

  class GetConnectedDevicesResponse : public ResponsePacket<GetConnectedDevicesResponse> {

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

    GetConnectedDevicesResponse(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(MGMTFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

  private:
    std::vector<std::string> m_devices;

  };

}

#endif //BABLE_LINUX_GETCONNECTEDDEVICESRESPONSE_HPP