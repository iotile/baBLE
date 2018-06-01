#ifndef BABLE_LINUX_REMOVEDEVICEREQUEST_HPP
#define BABLE_LINUX_REMOVEDEVICEREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet::Commands {

  class RemoveDeviceRequest : public RequestPacket<RemoveDeviceRequest> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      switch(type) {
        case Packet::Type::MGMT:
          return Format::MGMT::CommandCode::RemoveDevice;

        case Packet::Type::HCI:
          throw std::invalid_argument("'RemoveDevice' packet is not compatible with HCI protocol.");

        case Packet::Type::ASCII:
          return Format::Ascii::CommandCode::RemoveDevice;

        case Packet::Type::FLATBUFFERS:
          return static_cast<uint16_t>(BaBLE::Payload::RemoveDevice);

        case Packet::Type::NONE:
          return 0;
      }
    };

    RemoveDeviceRequest(Packet::Type initial_type, Packet::Type translated_type);

    void unserialize(AsciiFormatExtractor& extractor) override;
    void unserialize(FlatbuffersFormatExtractor& extractor) override;

    std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
    std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

  private:
    uint8_t m_address_type;
    std::string m_address;
    std::array<uint8_t, 6> m_raw_address{};

  };

}

#endif //BABLE_LINUX_REMOVEDEVICEREQUEST_HPP
