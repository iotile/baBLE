#ifndef BABLE_LINUX_DISCONNECT_HPP
#define BABLE_LINUX_DISCONNECT_HPP

#include "../../Base/HostToControllerPacket.hpp"

namespace Packet {

  namespace Commands {

    class Disconnect : public HostToControllerPacket {

    public:
      static const Packet::Type final_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return static_cast<uint16_t>(BaBLE::Payload::Disconnect);
      };

      static const uint16_t final_packet_code() {
        return Format::HCI::CommandCode::Disconnect;
      };

      explicit Disconnect(uint8_t m_reason = Format::HCI::StatusCode::ConnectionTerminatedLocalHost);

      void unserialize(FlatbuffersFormatExtractor& extractor) override;
      std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const override;

      const std::string stringify() const override;

    private:
      uint8_t m_reason;

    };

  }

}

#endif //BABLE_LINUX_DISCONNECT_HPP
