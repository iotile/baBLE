#ifndef BABLE_LINUX_ROUTER_HPP
#define BABLE_LINUX_ROUTER_HPP

#include <cstdint>
#include <memory>
#include <unordered_map>
#include "../AbstractSocket.hpp"
#include "../../Packet/AbstractPacket.hpp"
#include "../../Exceptions/NotFound/NotFoundException.hpp"

class SocketContainer {

public:
  // Register given socket in container
  SocketContainer& register_socket(std::shared_ptr<AbstractSocket> socket);

  // Send a packet using the matching registered socket
  bool send(std::unique_ptr<Packet::AbstractPacket> packet);

private:
  std::unordered_map<Packet::Type, std::shared_ptr<AbstractSocket>> m_sockets;

};

#endif //BABLE_LINUX_ROUTER_HPP
