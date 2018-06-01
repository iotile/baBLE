#ifndef BABLE_LINUX_GETCONTROLLERINFORESPONSE_HPP
#define BABLE_LINUX_GETCONTROLLERINFORESPONSE_HPP

#include "../ResponsePacket.hpp"

namespace Packet {

  namespace Commands {

    class GetControllerInfoResponse : public ResponsePacket<GetControllerInfoResponse> {

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

      GetControllerInfoResponse(Packet::Type initial_type, Packet::Type translated_type);

      Format::MGMT::Controller get_controller_info() const {
        return m_controller_info;
      }

      void unserialize(MGMTFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

    private:
      Format::MGMT::Controller m_controller_info;

    };

  }

}

#endif //BABLE_LINUX_GETCONTROLLERINFORESPONSE_HPP
