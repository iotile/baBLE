#ifndef BABLE_LINUX_ADDDEVICERESPONSE_HPP
#define BABLE_LINUX_ADDDEVICERESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    class AddDeviceResponse : public ResponsePacket<AddDeviceResponse> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::AddDevice;

          case Packet::Type::HCI:
            throw std::invalid_argument("'AddDevice' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::CommandCode::AddDevice;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::AddDevice);

          case Packet::Type::NONE:
            return 0;
        }
      };

      AddDeviceResponse(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    private:
      uint8_t m_address_type;
      std::array<uint8_t, 6> m_raw_address{};
      std::string m_address;

    };

  }

}

#endif //BABLE_LINUX_ADDDEVICERESPONSE_HPP
