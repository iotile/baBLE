#ifndef BABLE_LINUX_COMMANDPACKET_HPP
#define BABLE_LINUX_COMMANDPACKET_HPP

#include "../AbstractPacket.hpp"
#include "../../Serializer/Serializer.hpp"
#include "../constants.hpp"

namespace Packet::Commands {

  class CommandPacket : public AbstractPacket {

  protected:
    CommandPacket(Packet::Type initial_type, Packet::Type translated_type): AbstractPacket(initial_type, translated_type) {
      m_command_code = 0;
      m_controller_id = 0xFFFF;
      m_params_length = 0;
    };

    void generate_header(Serializer& ser) const {
      switch(m_current_type) {
        case Packet::MGMT:
          ser << m_command_code << m_controller_id << m_params_length;
          break;

        default:
          throw std::runtime_error("Can't generate header for current packet type.");
      }
    }

    void set_controller(uint16_t controller_id) {
      m_controller_id = controller_id;
    }

    uint16_t m_command_code;
    uint16_t m_controller_id;
    uint16_t m_params_length;

  };

}

#endif //BABLE_LINUX_COMMANDPACKET_HPP
