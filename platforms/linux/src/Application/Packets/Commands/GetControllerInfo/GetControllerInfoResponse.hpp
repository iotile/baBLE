#ifndef BABLE_LINUX_GETCONTROLLERINFORESPONSE_HPP
#define BABLE_LINUX_GETCONTROLLERINFORESPONSE_HPP

#include "../../Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetControllerInfoResponse : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return Format::MGMT::CommandCode::GetControllerInfo;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::GetControllerInfo);
      };

      GetControllerInfoResponse();

      Format::MGMT::Controller get_controller_info() const {
        return m_controller_info;
      }

      void unserialize(MGMTFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      Format::MGMT::Controller m_controller_info;

    };

  }

}

#endif //BABLE_LINUX_GETCONTROLLERINFORESPONSE_HPP
