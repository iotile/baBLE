#ifndef BABLE_LINUX_COMMANDCOMPLETE_HPP
#define BABLE_LINUX_COMMANDCOMPLETE_HPP

#include "../EventPacket.hpp"

namespace Packet {

  namespace Events {

    class CommandComplete : public EventPacket<CommandComplete> {

    public:
      static const uint16_t packet_code(Packet::Type type) {
        switch (type) {
          case Packet::Type::MGMT:
            throw std::invalid_argument("'CommandComplete' packet is not compatible with HCI protocol.");

          case Packet::Type::HCI:
            return Format::HCI::EventCode::CommandComplete;

          case Packet::Type::FLATBUFFERS:
            throw std::invalid_argument("'CommandComplete' event has no event code for Flatbuffers.");

          case Packet::Type::NONE:
            return 0;
        }
      };

      CommandComplete(Packet::Type initial_type, Packet::Type translated_type);

      void unserialize(HCIFormatExtractor& extractor) override;

      const std::string stringify() const override;

      const PacketUuid get_uuid() const override {
        return PacketUuid{
            m_current_type,
            m_controller_id,
            m_connection_id,
            m_packet_code,
            get_opcode()
        };
      }

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
