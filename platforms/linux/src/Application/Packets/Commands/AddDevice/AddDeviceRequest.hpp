#ifndef BABLE_LINUX_ADDDEVICEREQUEST_HPP
#define BABLE_LINUX_ADDDEVICEREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class AddDeviceRequest : public RequestPacket<AddDeviceRequest> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::AddDevice;

          case Packet::Type::HCI:
            throw std::invalid_argument("'AddDevice' packet is not compatible with HCI protocol.");

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::AddDevice);

          case Packet::Type::NONE:
            return 0;
        }
      };

      AddDeviceRequest(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      std::string m_address;
      std::array<uint8_t, 6> m_raw_address{};
      uint8_t m_address_type;
      uint8_t m_action;

    };

  }

}

#endif //BABLE_LINUX_ADDDEVICEREQUEST_HPP
