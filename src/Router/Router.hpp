#ifndef BABLE_LINUX_ROUTER_HPP
#define BABLE_LINUX_ROUTER_HPP

#include "../Socket/AbstractSocket.hpp"
#include "../Packet/constants.hpp"
#include "../Format/AbstractFormat.hpp"

class Router {

public:
  Router& register_socket(std::shared_ptr<AbstractSocket> socket, std::shared_ptr<AbstractFormat> format) {
    Packet::Type packet_type = format->packet_type();
    m_sockets.emplace(packet_type, std::move(socket));
    m_formats.emplace(packet_type, std::move(format));

    return *this;
  }

  void send(std::unique_ptr<Packet::AbstractPacket> packet) {
    Packet::Type type = packet->get_type();

    auto it_socket = m_sockets.find(type);
    if (it_socket == m_sockets.end()) {
      throw std::invalid_argument("Can't find socket in Router for given packet type: " + std::to_string(type));
    }
    std::shared_ptr<AbstractSocket> socket = it_socket->second;

    auto it_format = m_formats.find(type);
    if (it_format == m_formats.end()) {
      throw std::invalid_argument("Can't find format in Router for given packet type: " + std::to_string(type));
    }
    std::shared_ptr<AbstractFormat> format = it_format->second;

//    std::vector<uint8_t> data = packet->serialize(format);
//    socket->send(data);
    socket->send(std::move(packet));
  };

private:
  std::unordered_map<Packet::Type, std::shared_ptr<AbstractSocket>> m_sockets;
  std::unordered_map<Packet::Type, std::shared_ptr<AbstractFormat>> m_formats;

};

#endif //BABLE_LINUX_ROUTER_HPP
