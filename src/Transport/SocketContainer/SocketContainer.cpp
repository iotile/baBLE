#include "SocketContainer.hpp"

SocketContainer& SocketContainer::register_socket(std::shared_ptr<AbstractSocket> socket) {
  Packet::Type packet_type = socket->format()->packet_type();
  m_sockets.emplace(packet_type, std::move(socket));

  return *this;
}

bool SocketContainer::send(std::unique_ptr<Packet::AbstractPacket> packet) {
  Packet::Type type = packet->current_type();

  auto it = m_sockets.find(type);
  if (it == m_sockets.end()) {
    throw Exceptions::NotFoundException("Can't find socket in SocketContainer for given packet type.");
  }
  std::shared_ptr<AbstractSocket> socket = it->second;

  std::vector<uint8_t> data = packet->serialize();
  return socket->send(data);
};
