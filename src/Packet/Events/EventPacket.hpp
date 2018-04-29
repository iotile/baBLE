#ifndef BABLE_LINUX_EVENTPACKET_HPP
#define BABLE_LINUX_EVENTPACKET_HPP

#include "../AbstractPacket.hpp"
#include "../../Serializer/Deserializer.hpp"

namespace Packet::Events {

  template<class T>
  class EventPacket : public AbstractPacket {

  protected:
    EventPacket(Packet::Type initial_type, Packet::Type translated_type): AbstractPacket(initial_type, translated_type) {
      m_event_code = 0;
      m_controller_id = 0xFFFF;
      m_params_length = 0;
    };

    std::string to_ascii() const override {
      std::stringstream result;
      generate_header(result);

      return result.str();
    };

    void from_mgmt(Deserializer& deser) override {
      deser >> m_event_code >> m_controller_id >> m_params_length;
    };

    uint16_t m_event_code;
    uint16_t m_controller_id;
    uint16_t m_params_length;

  private:
    void generate_header(std::stringstream& str) const {
      switch(m_current_type) {
        case Packet::ASCII:
          str << "<EventPacket> "
              << "Event code: " << std::to_string(m_event_code) << ", "
              << "Controller ID: " << std::to_string(m_controller_id) << ", "
              << "Parameters length: " << std::to_string(m_params_length);
          break;

        default:
          throw std::runtime_error("Can't generate header for current packet type.");
      }
    }

  };

}

#endif //BABLE_LINUX_EVENTPACKET_HPP
