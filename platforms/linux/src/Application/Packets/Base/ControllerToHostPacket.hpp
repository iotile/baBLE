#ifndef BABLE_CONTROLLERTOHOSTPACKET_HPP
#define BABLE_CONTROLLERTOHOSTPACKET_HPP

#include "Application/Packets/AbstractPacket.hpp"

namespace Packet {

  class ControllerToHostPacket : public AbstractPacket {

  public:
    static Packet::Type final_type() {
      return Packet::Type::FLATBUFFERS;
    };

  protected:
    ControllerToHostPacket(Packet::Id id, Packet::Type type, uint16_t initial_packet_code,
                           uint16_t final_packet_code, bool ignore = false);

    void prepare(const std::shared_ptr<PacketRouter>& router) override;

  private:
    uint16_t m_final_packet_code;

  };

}

#endif //BABLE_CONTROLLERTOHOSTPACKET_HPP