#ifndef BABLE_LINUX_RESPONSEPACKET_HPP
#define BABLE_LINUX_RESPONSEPACKET_HPP

#include "../AbstractPacket.hpp"
#include "../../Serializer/Serializer.hpp"

namespace Packet::Responses {

  class ResponsePacket : public AbstractPacket {

  protected:
    ResponsePacket(Packet::Type initial_type, Packet::Type translated_type): AbstractPacket(initial_type, translated_type) {
      m_event_code = 0;
      m_controller_id = 0xFFFF;
      m_params_length = 0;
      m_command_code = 0;
      m_status = 0;
    };

    void generate_header(std::stringstream& str) const {
      switch(m_current_type) {
        case Packet::ASCII:
          str << "<ASCII Packet> "
                << "Event code: " << std::to_string(m_event_code) << ", "
                << "Controller ID: " << std::to_string(m_controller_id) << ", "
                << "Parameters length: " << std::to_string(m_params_length) << ", "
                << "Command code: " << std::to_string(m_command_code) << ", "
                << "Status: " << std::to_string(m_status) << ", ";
          break;

        default:
          throw std::runtime_error("Can't generate header for current packet type.");
      }
    }

    void from_mgmt(Deserializer& deser) override {
      deser >> m_event_code >> m_controller_id >> m_params_length >> m_command_code >> m_status;
    };

    uint16_t m_event_code;
    uint16_t m_controller_id;
    uint16_t m_params_length;
    uint16_t m_command_code;
    uint8_t m_status;

  };

}

#endif //BABLE_LINUX_RESPONSEPACKET_HPP
