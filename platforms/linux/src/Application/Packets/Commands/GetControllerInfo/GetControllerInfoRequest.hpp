#ifndef BABLE_LINUX_GETCONTROLLERINFOREQUEST_HPP
#define BABLE_LINUX_GETCONTROLLERINFOREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetControllerInfoRequest : public RequestPacket<GetControllerInfoRequest> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::GetControllerInfo;

          case Packet::Type::HCI:
            throw std::invalid_argument("'GetControllerInfo' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::CommandCode::GetControllerInfo;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::GetControllerInfo);

          case Packet::Type::NONE:
            return 0;
        }
      };

      GetControllerInfoRequest(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(AsciiFormatExtractor& extractor) override;
      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

    };

  }

}

#endif //BABLE_LINUX_GETCONTROLLERINFOREQUEST_HPP
