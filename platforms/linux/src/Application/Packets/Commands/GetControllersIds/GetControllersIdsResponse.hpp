#ifndef BABLE_LINUX_GETCONTROLLERSIDSRESPONSE_HPP
#define BABLE_LINUX_GETCONTROLLERSIDSRESPONSE_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Commands {

    class GetControllersIdsResponse : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return Format::MGMT::CommandCode::GetControllersList;
      };

      static const uint16_t final_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::GetControllersIds);
      };

      GetControllersIdsResponse();

      void unserialize(MGMTFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const override;

      const std::string stringify() const override;

      inline const std::vector<uint16_t>& get_controllers_ids() const {
        return m_controllers_ids;
      }

    private:
      std::vector<uint16_t> m_controllers_ids;

    };

  }

}

#endif //BABLE_LINUX_GETCONTROLLERSIDSRESPONSE_HPP
