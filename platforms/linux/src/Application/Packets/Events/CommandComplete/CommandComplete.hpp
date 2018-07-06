#ifndef BABLE_LINUX_COMMANDCOMPLETE_HPP
#define BABLE_LINUX_COMMANDCOMPLETE_HPP

#include "Application/Packets/Base/ControllerToHostPacket.hpp"

namespace Packet {

  namespace Events {

    class CommandComplete : public ControllerToHostPacket {

    public:
      static const Packet::Type initial_type() {
        return Packet::Type::HCI;
      };

      static const uint16_t initial_packet_code() {
        return Format::HCI::EventCode::CommandComplete;
      };

      static const uint16_t final_packet_code() {
        return initial_packet_code();
      };

      CommandComplete();

      void unserialize(HCIFormatExtractor& extractor) override;

      const std::string stringify() const override;

      const PacketUuid get_uuid() const override;

      inline uint16_t get_opcode() const {
        return m_opcode;
      };

      inline std::vector<uint8_t> get_returned_params() const {
        return m_returned_params;
      };

    private:
      uint16_t m_opcode;
      std::vector<uint8_t> m_returned_params;

    };

  }

}

#endif //BABLE_LINUX_COMMANDCOMPLETE_HPP
