#ifndef BABLE_LINUX_SOCKETMANAGER_HPP
#define BABLE_LINUX_SOCKETMANAGER_HPP

#include <memory>
#include <unordered_map>
#include "AbstractSocket.hpp"
#include "../Packet/constants.hpp"

class SocketManager {

public:
  SocketManager& register_socket(std::shared_ptr<AbstractSocket> socket) {
    Packet::Type type = socket->packet_type();
    m_sockets[type] = std::move(socket);

    return *this;
  }

  void send(std::unique_ptr<Packet::AbstractPacket> packet) {
    Packet::Type type = packet->get_type();

    auto it = m_sockets.find(type);
    if (it == m_sockets.end()) {
      throw std::invalid_argument("Packet to send has no known type.");
    }

    std::shared_ptr<AbstractSocket> socket = it->second;
    socket->send(std::move(packet));
  };

private:
  std::unordered_map<Packet::Type, std::shared_ptr<AbstractSocket>> m_sockets;

};

#endif //BABLE_LINUX_SOCKETMANAGER_HPP
