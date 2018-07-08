#include "ControllerToHostPacket.hpp"

namespace Packet {

  ControllerToHostPacket::ControllerToHostPacket(Packet::Id id, Packet::Type type, uint16_t initial_packet_code,
                                                 uint16_t final_packet_code, bool ignore)
    : AbstractPacket(id, type, final_type(), initial_packet_code) {
    switch (type) {
      case Packet::Type::HCI:
        m_native_class = "HCI";
        break;

      case Packet::Type::MGMT:
        m_native_class = "MGMT";
        break;

      default:
        m_native_class = "BaBLE";
        break;
    }

    m_final_packet_code = final_packet_code;

    if (ignore) {
      m_final_type = Packet::Type::NONE;
    }
  }

  void ControllerToHostPacket::prepare(const std::shared_ptr<PacketRouter>& router) {
    AbstractPacket::translate();
    m_packet_code = m_final_packet_code;
  }

}