#ifndef BABLE_LINUX_GETCONTROLLERINFOREQUEST_HPP
#define BABLE_LINUX_GETCONTROLLERINFOREQUEST_HPP

#include "../../Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetControllerInfoRequest : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::GetControllerInfo);
      };

      static const uint16_t final_packet_code() {
        return Format::MGMT::CommandCode::GetControllerInfo;
      };

      explicit GetControllerInfoRequest();

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

      const std::string stringify() const override;

    };

  }

}

#endif //BABLE_LINUX_GETCONTROLLERINFOREQUEST_HPP
