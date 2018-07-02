#ifndef BABLE_LINUX_SETDISCOVERABLEREQUEST_HPP
#define BABLE_LINUX_SETDISCOVERABLEREQUEST_HPP

#include "Application/Packets/Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class SetDiscoverableRequest : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::MGMT;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::SetDiscoverable);
      };

      static const uint16_t final_packet_code() {
        return Format::MGMT::CommandCode::SetDiscoverable;
      };

      explicit SetDiscoverableRequest(bool state = false, uint16_t timeout = 0);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;

      std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      bool m_state;
      uint16_t m_timeout;

    };

  }

}

#endif //BABLE_LINUX_SETDISCOVERABLEREQUEST_HPP
