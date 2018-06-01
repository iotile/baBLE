#ifndef BABLE_LINUX_GETCONTROLLERSIDSREQUEST_HPP
#define BABLE_LINUX_GETCONTROLLERSIDSREQUEST_HPP

#include "../RequestPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetControllersIdsRequest : public RequestPacket<GetControllersIdsRequest> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            return Format::MGMT::CommandCode::GetControllersList;

          case Packet::Type::HCI:
            throw std::invalid_argument("'GetControllersIds' packet is not compatible with HCI protocol.");

          case Packet::Type::ASCII:
            return Format::Ascii::CommandCode::GetControllersIds;

          case Packet::Type::FLATBUFFERS:
            return static_cast<uint16_t>(BaBLE::Payload::GetControllersIds);

          case Packet::Type::NONE:
            return 0;
        }
      };

      GetControllersIdsRequest(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(AsciiFormatExtractor& extractor) override;
      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

    };

  }

}

#endif //BABLE_LINUX_GETCONTROLLERSIDSREQUEST_HPP
