#ifndef BABLE_LINUX_SOCKETCONTAINER_HPP
#define BABLE_LINUX_SOCKETCONTAINER_HPP

#include <map>
#include "../AbstractSocket.hpp"
#include "../../Application/AbstractPacket.hpp"

class SocketContainer {

public:
  // Register given socket in container
  SocketContainer& register_socket(std::shared_ptr<AbstractSocket> socket);

  // Send a packet using the matching registered socket
  bool send(const std::shared_ptr<Packet::AbstractPacket>& packet);

private:
  std::map<std::tuple<Packet::Type, uint16_t>, std::shared_ptr<AbstractSocket>> m_sockets;

};

#endif //BABLE_LINUX_SOCKETCONTAINER_HPP
