#ifndef BABLE_LINUX_EVENTPACKET_HPP
#define BABLE_LINUX_EVENTPACKET_HPP

#include "../../AbstractPacket.hpp"

namespace Packet {

  namespace Events {

    template<class T>
    class EventPacket : public AbstractPacket {

    protected:
      EventPacket(Packet::Type initial_type, Packet::Type translated_type)
          : AbstractPacket(initial_type, translated_type) {
        m_packet_code = T::packet_code(m_current_type);
      };

    };

  }

}

#endif //BABLE_LINUX_EVENTPACKET_HPP