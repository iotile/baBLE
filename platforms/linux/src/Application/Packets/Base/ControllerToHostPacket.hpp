#ifndef BABLE_LINUX_CONTROLLERTOHOSTPACKET_HPP
#define BABLE_LINUX_CONTROLLERTOHOSTPACKET_HPP

#include "../AbstractPacket.hpp"

namespace Packet {

  class ControllerToHostPacket : public AbstractPacket {

  public:
    static Packet::Type final_type() {
      return Packet::Type::FLATBUFFERS;
    };

  protected:
    ControllerToHostPacket(Packet::Id id, Packet::Type type, uint16_t initial_packet_code, uint16_t final_packet_code, bool ignore = false)
        : AbstractPacket(id, type, final_type(), initial_packet_code) {
      m_final_packet_code = final_packet_code;

      if (ignore) {
        m_final_type = Packet::Type::NONE;
      }
    };

    void prepare(const std::shared_ptr<PacketRouter>& router) override {
      AbstractPacket::translate();
      m_packet_code = m_final_packet_code;
    };

  private:
    uint16_t m_final_packet_code;

  };

}

#endif //BABLE_LINUX_CONTROLLERTOHOSTPACKET_HPP