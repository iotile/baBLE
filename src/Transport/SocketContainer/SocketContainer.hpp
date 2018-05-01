#ifndef BABLE_LINUX_ROUTER_HPP
#define BABLE_LINUX_ROUTER_HPP

#include <cstdint>
#include <memory>
#include <unordered_map>
#include "../AbstractSocket.hpp"

class SocketContainer {

public:
  SocketContainer& register_socket(std::shared_ptr<AbstractSocket> socket) {
    Packet::Type packet_type = socket->format()->packet_type();
    m_sockets.emplace(packet_type, std::move(socket));

    return *this;
  }

  void send(std::unique_ptr<Packet::AbstractPacket> packet) {
    Packet::Type type = packet->current_type();

    auto it = m_sockets.find(type);
    if (it == m_sockets.end()) {
      throw std::invalid_argument("Can't find socket in SocketContainer for given packet type: " + std::to_string(type));
    }
    std::shared_ptr<AbstractSocket> socket = it->second;

    std::vector<uint8_t> data = packet->serialize();
    socket->send(data);
  };

private:
  std::unordered_map<Packet::Type, std::shared_ptr<AbstractSocket>> m_sockets;

};

#endif //BABLE_LINUX_ROUTER_HPP
