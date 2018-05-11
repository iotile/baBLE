#include "SocketContainer.hpp"

using namespace std;

SocketContainer& SocketContainer::register_socket(shared_ptr<AbstractSocket> socket) {
  Packet::Type packet_type = socket->format()->packet_type();
  m_sockets.emplace(packet_type, move(socket));

  return *this;
}

bool SocketContainer::send(unique_ptr<Packet::AbstractPacket> packet) {
  Packet::Type type = packet->current_type();

  if (type == Packet::Type::NONE) {
    LOG.debug("Packet ignored: " + packet->stringify());
    return true;
  }

  auto it = m_sockets.find(type);
  if (it == m_sockets.end()) {
    throw Exceptions::NotFoundException("Can't find socket in SocketContainer for given packet type.");
  }
  shared_ptr<AbstractSocket> socket = it->second;

  vector<uint8_t> data = packet->serialize();
  return socket->send(data);
};
