#ifndef BABLE_LINUX_RESPONSEPACKET_HPP
#define BABLE_LINUX_RESPONSEPACKET_HPP

#include "../../AbstractPacket.hpp"

namespace Packet {

  namespace Commands {

    template<class T>
    class ResponsePacket : public AbstractPacket {

    protected:
      ResponsePacket(Packet::Type initial_type, Packet::Type translated_type)
          : AbstractPacket(initial_type, translated_type) {
        m_packet_code = T::packet_code(m_current_type);
      };

      void before_sent(const std::shared_ptr<PacketRouter>& router) override {
        AbstractPacket::before_sent(router);
        m_packet_code = T::packet_code(m_current_type);
      };

      void unserialize(MGMTFormatExtractor& extractor) override {
        set_status(extractor.get_value<uint8_t>());
      };

    };

  }

}

#endif //BABLE_LINUX_RESPONSEPACKET_HPP