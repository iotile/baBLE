#include "HostToControllerPacket.hpp"

using namespace std;

namespace Packet {

  PacketUuid HostToControllerPacket::get_response_uuid() const {
    PacketUuid response_uuid = get_uuid();
    response_uuid.response_packet_code = m_response_packet_code;

    return response_uuid;
  }

  HostToControllerPacket::HostToControllerPacket(Packet::Id id, Packet::Type type, uint16_t packet_code, bool waiting_response)
    : AbstractPacket(id, initial_type(), type, packet_code) {
    m_response_packet_code = m_packet_code;
    m_waiting_response = waiting_response;
  }

  void HostToControllerPacket::set_waiting_response(bool waiting_response) {
    m_waiting_response = waiting_response;
  }

  void HostToControllerPacket::prepare(const shared_ptr<PacketRouter>& router) {
    AbstractPacket::translate();
    
    if (m_waiting_response) {
    PacketUuid response_uuid = get_response_uuid();
    auto response_callback =
        [this](const shared_ptr<PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
          return on_response_received(router, packet);
        };
    router->add_callback(response_uuid, shared_from(this), response_callback);
    }
  }

  vector<uint8_t> HostToControllerPacket::serialize(MGMTFormatBuilder& builder) const {
    builder.set_opcode(m_packet_code);
    
    return {};
  }
  
  vector<uint8_t> HostToControllerPacket::serialize(HCIFormatBuilder& builder) const {
  builder.set_opcode(m_packet_code);
  
  return {};
  }

  shared_ptr<Packet::AbstractPacket> HostToControllerPacket::on_response_received(const shared_ptr<PacketRouter>& router,
                                                                                  const shared_ptr<AbstractPacket>& packet) {
    LOG.debug("Response received", "RequestPacket");
    packet->set_uuid_request(m_uuid_request);
    return packet;
  }
  
}
