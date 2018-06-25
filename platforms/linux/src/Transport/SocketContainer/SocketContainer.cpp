#include "SocketContainer.hpp"
#include "../../Log/Log.hpp"
#include "../../Exceptions/BaBLEException.hpp"

using namespace std;

SocketContainer& SocketContainer::register_socket(shared_ptr<AbstractSocket> socket) {
  Packet::Type packet_type = socket->format()->get_packet_type();
  m_sockets.emplace(make_tuple(packet_type, socket->get_controller_id()), move(socket));

  return *this;
}

bool SocketContainer::send(const shared_ptr<Packet::AbstractPacket>& packet) {
  Packet::Type packet_type = packet->get_type();

  if (packet_type == Packet::Type::NONE) {
    LOG.debug("Packet ignored: " + packet->stringify());
    return true;
  }

  tuple<Packet::Type, uint16_t> key;
  if (packet_type == Packet::Type::HCI) {
    key = make_tuple(packet_type, packet->get_controller_id());
  } else {
    key = make_tuple(packet_type, NON_CONTROLLER_ID);
  }

  auto it = m_sockets.find(key);
  if (it == m_sockets.end()) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::NotFound,
        "Can't find socket in SocketContainer for given packet.",
        packet->get_uuid_request()
    );
  }
  shared_ptr<AbstractSocket> socket = it->second;

  vector<uint8_t> data = packet->to_bytes();

  try {
    bool result = socket->send(data);
    return result;

  } catch (Exceptions::BaBLEException& err) {
    err.set_uuid_request(packet->get_uuid_request());
    throw;
  }
};
