#ifndef BABLE_LINUX_COMMANDSTATUS_HPP
#define BABLE_LINUX_COMMANDSTATUS_HPP

#include "../../Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Events {

    class CommandStatus : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::EventCode::CommandStatus;
      };

      static const uint16_t final_packet_code() {
        return initial_packet_code();
      };

      CommandStatus();

      void unserialize(HCIFormatExtractor& extractor) override;

      const std::string stringify() const override;

      const PacketUuid get_uuid() const override;

      inline uint16_t get_opcode() const {
        return m_opcode;
      };

    private:
      uint16_t m_opcode;

    };

  }

}

#endif //BABLE_LINUX_COMMANDSTATUS_HPP
